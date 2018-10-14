//
// Created by Wolf on 10.10.2018.
//

#ifndef IFJ_PROJECT_SCANNER_H
#define IFJ_PROJECT_SCANNER_H

#endif //IFJ_PROJECT_SCANNER_H


#define KEYWORD_COUNT  9

typedef enum {

    //KEYWORDS
    IF, ELSE, DEF, DO, END, NOT, NIL, THEN, WHILE,

    //DATA
    INT, FLOAT, STRING,


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

