//
// Created by Wolf on 10.10.2018.
//

#ifndef IFJ_PROJECT_SCANNER_H
#define IFJ_PROJECT_SCANNER_H

#endif //IFJ_PROJECT_SCANNER_H


#define KEYWORD_COUNT  9


typedef enum {

    //KEYWORDS
    IF = 0, ELSE, DEF, DO, END, NOT, NIL, THEN, WHILE, //012345678

    //STATES
    WORD = 20, NUMBER, EQUALCASES, LESSCASES, MORECASES, KEYWORDCASES, //20,21,22,23,24,25
    EOL_CASE = 500, EOF_CASE, ERROR, //500, 501, 502

    //DATA
    INT = 1000, FLOAT, STRING, FLOAT_EXPO, IDENTIFICATOR, //1000, 1001, 1002, 1003, 1004


    //OPERATORS
    PLUS = 100, // + 100
    MINUS,      // - 101
    MULT,       // * 102
    DIVIDE,     // / 103
    ASSIGN,     // = 104


    //LOGIC
    LESS,       // < 105
    MORE,       // > 106
    LOE,        // <=107
    MOE,        // >=108
    EQUAL,      // ==109
    NOTEQUAL,   // !=110


    //SYMBOLS
    LBRA,       // { 111
    RBRA,       // } 112
    SEMICOLON,  // ; 113
    LPAR,       // ( 114
    RPAR,       // ) 115

    //COMMENTS
    SINGLECOM=200,   // # -SINGLE-LINE COMMENTS   200
    BEGCOM,      // =begin - BLOCK COMMENTS BEGIN 201
    ENDCOM,       // =end   - BLOCK COMMENTS END  203
} Tokens_Types;

typedef struct
{
    Tokens_Types type;
    union
    {
        int    value_int;
        double value_double;
        char*  string;
    }data;
}tToken;