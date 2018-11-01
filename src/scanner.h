//
// Created by Wolf on 10.10.2018.
//

#ifndef IFJ_PROJECT_SCANNER_H
#define IFJ_PROJECT_SCANNER_H

#endif //IFJ_PROJECT_SCANNER_H


#define KEYWORD_COUNT  9


typedef enum {
    //STATES
    WORD, NUMBER, EQUALCASES, LESSCASES, MORECASES, PLUSCASES, MINUSCASES, KEYWORDCASES, ERROR,

    //KEYWORDS
    IF, ELSE, DEF, DO, END, NOT, NIL, THEN, WHILE,

    //DATA
    INT, FLOAT, STRING, FLOAT_EXPO, IDENTIFICATOR,


    //OPERATORS
    PLUS,       // +
    MINUS,      // -
    MULT,       // *
    MODULO,     // %
    DIVIDE,     // /
    ASSIGN,     // =
    INC,        //++
    DEC,        //--
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

    //TERM
    ID,

    //COMMENTS
    SINGLECOM,   // # -SINGLE-LINE COMMENTS
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