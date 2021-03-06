FILE_TO_TEST=$PWD"/"$1
FUNCTION_TO_TEST=$2

parent_path=$( cd "$(dirname "${BASH_SOURCE[0]}")" ; pwd -P )
cd $parent_path
OPERATORS_ALLOWED="1"
PARSED_FILE="parsed_file.txt"
ARGUMENTS_FILE="arguments.txt"

PARSER_FILE="../src/python/parse_program.py" # this is the python script that will parse the c file
CPP_BINARY="../bin/run_mtt"


python3 $PARSER_FILE $FILE_TO_TEST $FUNCTION_TO_TEST $PARSED_FILE $ARGUMENTS_FILE
$CPP_BINARY $PARSED_FILE $OPERATORS_ALLOWED $ARGUMENTS_FILE

rm $PARSED_FILE
rm $ARGUMENTS_FILE