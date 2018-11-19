//
// Created by Wolf on 10.10.2018.
//

#ifndef IFJ_PROJECT_SCANNER_H
#define IFJ_PROJECT_SCANNER_H

#endif //IFJ_PROJECT_SCANNER_H


#define KEYWORD_COUNT  9


typedef enum {

    //KEYWORDS
    IF=0, ELSE, DEF, DO, END, NOT, NIL, THEN, WHILE,

    //STATES
    WORD, NUMBER, EQUALCASES, LESSCASES, MORECASES, KEYWORDCASES, EOL_CASE, EOF_CASE=500, ERROR,

    //DATA
    INT=1000, FLOAT, STRING, FLOAT_EXPO, IDENTIFICATOR,


    //OPERATORS
    PLUS,       // +
    MINUS=100,      // -
    MULT,       // *
    DIVIDE,     // /
    ASSIGN,     // =


    //LOGIC
    LESS,       // <
    MORE,       // >
    LOE,        // <=
    MOE,        // >=
    EQUAL,      // ==


    //SYMBOLS
    LBRA,       // {
    RBRA,       // }
    SEMICOLON,  // ;
    LPAR,       // (
    RPAR,       // )

    //COMMENTS
    SINGLECOM=200,   // # -SINGLE-LINE COMMENTS
    BEGCOM,      // =begin - BLOCK COMMENTS BEGIN
    ENDCOM,       // =end   - BLOCK COMMENTS END
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