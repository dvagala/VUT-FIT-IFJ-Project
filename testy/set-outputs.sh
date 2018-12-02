#!/bin/bash
echo "Setting outputs for testing..."
#keyword
./scanner-tests<inputs/test_key.txt>outputs/test_key 
#float
./scanner-tests<inputs/test_float.txt>outputs/test_float 
#float_errors
./scanner-tests<inputs/test_float_err.txt>outputs/test_float_err 
#int 
./scanner-tests<inputs/test_int.txt>outputs/test_int 
#function_id 
./scanner-tests<inputs/test_idfunk.txt>outputs/test_idfunk 
#id 
./scanner-tests<inputs/test_id.txt>outputs/test_id 
#id_errors 
./scanner-tests<inputs/test_id_err.txt>outputs/test_id_err 
#string
./scanner-tests<inputs/test_string.txt>outputs/tests_string 
#string_errors
./scanner-tests<inputs/test_string_err.txt>outputs/test_string_err 
#string1
./scanner-tests<inputs/test_string1.txt>outputs/test_string1 
#comments
./scanner-tests<inputs/test_com_err.txt>outputs/test_com_err_out 
./scanner-tests<inputs/test_com_err2.txt>outputs/test_com_err2 
./scanner-tests<inputs/test_com_err3.txt>outputs/test_com_err3 
./scanner-tests<inputs/test_comment.txt>outputs/test_comment 
echo "Outputs set."
exit 0