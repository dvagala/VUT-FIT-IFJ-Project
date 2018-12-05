/*
 * Project: Implementace překladače imperativního jazyka IFJ18
 *
 * File: errors.h
 *
 * xglosk00   Kristian Glos
 * xvagal00   Dominik Vagala
 * xvinsj00   Jakub Vins
 * xabike00   Zhamilya Abikenova
 *
 */

#ifndef IFJ_PROJECT_ERRORS_H
#define IFJ_PROJECT_ERRORS_H

#define COMPILE_OK 0                 //If there's is no error in compiling
#define LEX_ERROR 1                  //Error in lexical analysis
#define SYNTAX_ERROR 2               //Error in syntax analysis
#define SEMANTIC_ERROR 3             //Error in semantic analysis (undefined variable, function..)
#define COMPATIBILITY_ERROR 4        //Error in compatibility of data types
#define PARAM_ERROR 5                //Wrong number of parametersi in function
#define DIFF_ERROR 6                 //Other semantic errors
#define ZERO_ERROR 9                 //Zero division
#define INNER_ERROR 99               //Internal error (malloc failed..)

#endif //IFJ_PROJECT_ERRORS_H
