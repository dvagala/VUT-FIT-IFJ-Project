#!/bin/bash
echo "Setting outputs for testing..."
#keyword
./scanner-tests<inputs/test_key.txt>outputs/test_key.OUTPUT

#float
./scanner-tests<inputs/test_float.txt>outputs/test_float.out

#float_errors
./scanner-tests<inputs/test_float_err.txt>outputs/test_float_err.OUTPUT

#int 
./scanner-tests<inputs/test_int.txt>outputs/test_int.OUTPUT

#function_id 
./scanner-tests<inputs/test_idfunk.txt>outputs/test_idfunk.OUTPUT

#id 
./scanner-tests<inputs/test_id.txt>outputs/test_id.OUTPUT

#id_errors
./scanner-tests<inputs/test_id_err.txt>outputs/test_id_err.OUTPUT

#string
./scanner-tests<inputs/test_string.txt>outputs/teststring.OUTPUT

#string_errors
./scanner-tests<inputs/test_string_err.txt>outputs/test_string_err.OUTPUT

#string1
./scanner-tests<inputs/test_string1.txt>outputs/test_string1.OUTPUT
echo "Outputs set."
exit 0