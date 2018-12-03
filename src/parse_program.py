#------------------------------------------------------------------------------
# pycparser: c_json.py
#
# by Michael White (@mypalmike)
#
# This example includes functions to serialize and deserialize an ast
# to and from json format. Serializing involves walking the ast and converting
# each node from a python Node object into a python dict. Deserializing
# involves the opposite conversion, walking the tree formed by the
# dict and converting each dict into the specific Node object it represents.
# The dict itself is serialized and deserialized using the python json module.
#
# The dict representation is a fairly direct transformation of the object
# attributes. Each node in the dict gets one metadata field referring to the
# specific node class name, _nodetype. Each local attribute (i.e. not linking
# to child nodes) has a string value or array of string values. Each child
# attribute is either another dict or an array of dicts, exactly as in the
# Node object representation. The "coord" attribute, representing the
# node's location within the source code, is serialized/deserialized from
# a Coord object into a string of the format "filename:line[:column]".
#
# Example TypeDecl node, with IdentifierType child node, represented as a dict:
#     "type": {
#         "_nodetype": "TypeDecl",
#         "coord": "c_files/funky.c:8",
#         "declname": "o",
#         "quals": [],
#         "type": {
#             "_nodetype": "IdentifierType",
#             "coord": "c_files/funky.c:8",
#             "names": [
#                 "char"
#             ]
#         }
#     }
#------------------------------------------------------------------------------
from __future__ import print_function

import json
import sys
import re

# This is not required if you've installed pycparser into
# your site-packages/ with setup.py
#
sys.path.extend(['.', '..'])

from pycparser import parse_file, c_ast
from pycparser.plyparser import Coord


RE_CHILD_ARRAY = re.compile(r'(.*)\[(.*)\]')
RE_INTERNAL_ATTR = re.compile('__.*__')


class CJsonError(Exception):
    pass


def memodict(fn):
    """ Fast memoization decorator for a function taking a single argument """
    class memodict(dict):
        def __missing__(self, key):
            ret = self[key] = fn(key)
            return ret
    return memodict().__getitem__


@memodict
def child_attrs_of(klass):
    """
    Given a Node class, get a set of child attrs.
    Memoized to avoid highly repetitive string manipulation

    """
    non_child_attrs = set(klass.attr_names)
    all_attrs = set([i for i in klass.__slots__ if not RE_INTERNAL_ATTR.match(i)])
    return all_attrs - non_child_attrs


def to_dict(node):
    """ Recursively convert an ast into dict representation. """
    klass = node.__class__

    result = {}

    # Metadata
    result['_nodetype'] = klass.__name__

    # Local node attributes
    for attr in klass.attr_names:
        result[attr] = getattr(node, attr)

    # Coord object
    if node.coord:
        result['coord'] = str(node.coord)
    else:
        result['coord'] = None

    # Child attributes
    for child_name, child in node.children():
        # Child strings are either simple (e.g. 'value') or arrays (e.g. 'block_items[1]')
        match = RE_CHILD_ARRAY.match(child_name)
        if match:
            array_name, array_index = match.groups()
            array_index = int(array_index)
            # arrays come in order, so we verify and append.
            result[array_name] = result.get(array_name, [])
            if array_index != len(result[array_name]):
                raise CJsonError('Internal ast error. Array {} out of order. '
                    'Expected index {}, got {}'.format(
                    array_name, len(result[array_name]), array_index))
            result[array_name].append(to_dict(child))
        else:
            result[child_name] = to_dict(child)

    # Any child attributes that were missing need "None" values in the json.
    for child_attr in child_attrs_of(klass):
        if child_attr not in result:
            result[child_attr] = None

    return result


def to_json(node, **kwargs):
    """ Convert ast node to json string """
    return json.dumps(to_dict(node), **kwargs)


def file_to_dict(filename):
    """ Load C file into dict representation of ast """
    ast = parse_file(filename, use_cpp=True)
    return to_dict(ast)


def file_to_json(filename, **kwargs):
    """ Load C file into json string representation of ast """
    ast = parse_file(filename, use_cpp=True)
    return to_json(ast, **kwargs)


def _parse_coord(coord_str):
    """ Parse coord string (file:line[:column]) into Coord object. """
    if coord_str is None:
        return None

    vals = coord_str.split(':')
    vals.extend([None] * 3)
    filename, line, column = vals[:3]
    return Coord(filename, line, column)


def _convert_to_obj(value):
    """
    Convert an object in the dict representation into an object.
    Note: Mutually recursive with from_dict.

    """
    value_type = type(value)
    if value_type == dict:
        return from_dict(value)
    elif value_type == list:
        return [_convert_to_obj(item) for item in value]
    else:
        # String
        return value


def from_dict(node_dict):
    """ Recursively build an ast from dict representation """
    class_name = node_dict.pop('_nodetype')

    klass = getattr(c_ast, class_name)

    # Create a new dict containing the key-value pairs which we can pass
    # to node constructors.
    objs = {}
    for key, value in node_dict.items():
        if key == 'coord':
            objs[key] = _parse_coord(value)
        else:
            objs[key] = _convert_to_obj(value)

    # Use keyword parameters, which works thanks to beautifully consistent
    # ast Node initializers.
    return klass(**objs)


def from_json(ast_json):
    """ Build an ast from json string representation """
    return from_dict(json.loads(ast_json))


#------------------------------------------------------------------------------

def find_function_to_test(ast_dict, to_test):
    """ Finds the dict of the function named to_test in ast_dict """
    for node in ast_dict['ext']:
        if node['_nodetype'] == "FuncDef" and node['decl']['name'] == to_test:
            return node  

def add_variable_to_dict(variable_identifier, variable_type, variables_dict):
    """ node_json is the node with _nodetype == 'Decl' """
    if variable_identifier in variables_dict.keys():
        variables_dict[variable_identifier]['versions'] = variables_dict[variable_identifier]['versions'] + 1
    else:
        variable_json = {}
        variable_json['is_array'] = False
        variable_json['is_constant'] = False
        variable_json['data_type'] = variable_type
        variable_json['type'] = 'data'
        variable_json['versions'] = 1
        variable_json['type_theory'] = []
        variables_dict[variable_identifier] = variable_json

def add_constant_to_dict(constant_value, constant_type, variables_dict):
    if constant_value in variables_dict.keys():
        return
    else:
        constant_json = {}
        constant_json['data_type'] = constant_type
        constant_json['is_array'] = False
        constant_json['is_constant'] = True
        constant_json['type'] = 'constant'
        constant_json['versions'] = 1
        constant_json['type_theory'] = []
        variables_dict[constant_value] = constant_json

def add_array_to_dict(array_identifier, array_type, variables_dict):
    if array_identifier in variables_dict.keys():
        variables_dict[array_identifier]['versions'] = variables_dict[array_identifier]['versions'] + 1
    else:
        array_json = {}
        array_json['data_type'] = array_type
        array_json['is_array'] = True
        array_json['is_constant'] = False
        array_json['type'] = 'data'
        array_json['versions'] = 1
        array_json['type_theory'] = []
        variables_dict[array_identifier] = array_json

def handle_nodetype(node_json, variables_dict):
    if isinstance(node_json, type(None)) == True:
        return
    nodetype = node_json['_nodetype'] # gets the type of the node. For a list of supported nodetypes, see nodetypes.txt

    if nodetype == 'Decl' and node_json['type']['_nodetype'] == 'TypeDecl': # declaring a normal variable
        variable_identifier = node_json['name']
        variable_type = node_json['type']['type']['names'][0]
        add_variable_to_dict(variable_identifier, variable_type, variables_dict)
        handle_nodetype(node_json['init'], variables_dict)

    elif nodetype == 'Decl' and node_json['type']['_nodetype'] == 'ArrayDecl': # declaring an array
        array_identifier = node_json['type']['type']['declname']
        array_type = node_json['type']['type']['type']['names'][0]
        add_array_to_dict(array_identifier, array_type, variables_dict)
        intializer_list = node_json['init']['exprs']
        for node in intializer_list:
            handle_nodetype(node, variables_dict)

    elif nodetype == 'Assignment' and node_json['lvalue']['_nodetype'] == 'ArrayRef': # assigning to an array
        array_identifier = node_json['lvalue']['name']['name']
        add_array_to_dict(array_identifier, "", variables_dict) # array type not needed since it already exists in the dict
        handle_nodetype(node_json['rvalue'], variables_dict)

    elif nodetype == 'Assignment': # assigning to a normal variable
        variable_identifier = node_json['lvalue']['name']
        add_variable_to_dict(variable_identifier, "", variables_dict) # variable type is not needed since the variable already exists
        handle_nodetype(node_json['rvalue'], variables_dict)

    elif nodetype == 'If':
        if_true_node = node_json['iftrue']
        if_false_node = node_json['iffalse']
        handle_nodetype(if_true_node, variables_dict)
        handle_nodetype(if_false_node, variables_dict)

    elif nodetype == 'Compound':
        block_items = node_json['block_items']
        if isinstance(block_items, type(None)) == True:
            return
        for node in block_items:
            handle_nodetype(node, variables_dict)

    elif nodetype == 'BinaryOp':
        handle_nodetype(node_json['left'], variables_dict)
        handle_nodetype(node_json['right'], variables_dict)

    elif nodetype == 'Constant':
        add_constant_to_dict(node_json['value'], node_json['type'], variables_dict)

    elif nodetype == 'For':
        handle_nodetype(node_json['init']['decls'][0], variables_dict)
        handle_nodetype(node_json['cond'], variables_dict)
        handle_nodetype(node_json['stmt'], variables_dict)


def get_args_of_function(function_json):
    args_json_list = function_json['decl']['type']['args']['params'] # a list of arguments represented as jsons
    args_dict = {}
    for arg in args_json_list:
        variable_identifier = arg['name']
        variable_type = arg['type']['type']['names'][0]
        add_variable_to_dict(variable_identifier, variable_type, args_dict)
    return args_dict

def get_variables_of_function(function_json):
    variables_dict = get_args_of_function(function_json) # adds the arguments to the dictionary
    for node in function_json['body']['block_items']:
        handle_nodetype(node, variables_dict)
    return variables_dict

def add_types_to_dict(bound_index_tuples, bound_array_tuples, function_variables): # specifies whether each variable is data, constant or bound
    function_variables_keys = function_variables.keys()

    for bound_index in bound_index_tuples:
        if bound_index[0] in function_variables_keys and bound_index[1] in function_variables_keys:
            function_variables[bound_index[0]]['type'] = 'bound'
            function_variables[bound_index[1]]['type'] = 'index'

    for bound_array in bound_array_tuples:
        if bound_array[0] in function_variables_keys and bound_array[1] in function_variables_keys:
            # function_variables[bound_array[0]]['type'] = 'bound' # specify the variables representing bounds as such
            # TODO: figure this out:
            # since bound_array[0] is a constant found in the code, it could also be a constant. Thus is it safer to keep it as a data
            function_variables[bound_array[1]]['bound'] = int(bound_array[0]) # add a field containing the bound (or size) to each array

def print_function_variables(function_variables): # takes in a dict and outputs the contents in a the format 'name \n type \n datatype'
    for variable in function_variables.keys():
        number_of_versions = function_variables[variable]['versions']

        if function_variables[variable]['is_array'] == True:
            for array_version in range(number_of_versions):
                for array_index in range(function_variables[variable]['bound']):
                    print(variable + str(array_version) + str(array_index))
                    print(function_variables[variable]['type'])
                    print(function_variables[variable]['data_type'])
                    print()

        elif function_variables[variable]['is_constant'] == True:
            print(variable)
            print(function_variables[variable]['type'])
            print(function_variables[variable]['data_type'])
            print()

        else:
            for variable_version in range(number_of_versions):
                print(variable + str(variable_version))
                print(function_variables[variable]['type'])
                print(function_variables[variable]['data_type'])
                print()

def list_to_tuples(list):
    if len(list) == 0:
        return []
    tuples = []
    for item in list:
        split_item = item.split(',')
        tuples.append((split_item[0],split_item[1]))
    return tuples

if __name__ == "__main__":
    if len(sys.argv) > 2:
        file_name = sys.argv[1]
        function_name = sys.argv[2]
        ast_dict = file_to_dict(file_name)
        function_to_test = find_function_to_test(ast_dict, function_name)
        function_variables = get_args_of_function(function_to_test)
        # function_variables = get_variables_of_function(function_to_test)
        # print("Enter the names of all the variables representing indices and their loop bounds in the following format 'bound,index' separated by spaces")
        # bound_index_list = sys.stdin.readline().split('\n')[0].split(' ')
        # bound_index_tuples = list_to_tuples(bound_index_list)
        # print("Enter the values of all the constants representing array bounds in the following format 'bound,array' and separated by spaces, where bound is a constant")
        # bound_array_list = sys.stdin.readline().split('\n')[0].split(' ')
        # bound_array_tuples = list_to_tuples(bound_array_list)
        # add_types_to_dict(bound_index_tuples, bound_array_tuples, function_variables)
        print_function_variables(function_variables)
        # ast = from_dict(ast_dict)
        # print(to_json(ast, sort_keys=True, indent=4))
    else:
        print("Please provide a filename and a function as arguments")
