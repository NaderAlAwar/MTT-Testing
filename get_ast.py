from __future__ import print_function
import sys
from pycparser import c_parser, c_ast

fileToParse = sys.argv[1]

with open(fileToParse, 'r') as f:
	contents = f.readlines()
contents = ''.join(contents)
print(contents)
parser = c_parser.CParser()
ast = parser.parse(contents, filename='<none>')
ast.show(showcoord=True)
