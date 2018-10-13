//
// Created by Wolf on 10.10.2018.
//

#ifndef IFJ_PROJECT_SCANNER_H
#define IFJ_PROJECT_SCANNER_H

#endif //IFJ_PROJECT_SCANNER_H


#define TOKEN

typedef enum {

    //KEYWORDS
    IF, ELSE, DEF, DO, END, NOT, NIL, THEN, WHILE,

    //DATA
    INT, DOUBLE, STRING,


    //OPERATORS
    PLUS,       // +
    MINUS,      // -
    MULT,       // *
    MODULO,     // %
    DIVIDE,     // /
    ASSIGN,     // =

    //LOGIC
    LESS,       // <
    MORE,       // >
    EQUAL,      // ==


    //SYMBOLS
    LBRA,       // {
    RBRA,       // }
    SEMICOLON,  // ;
    LPAR,       // (
    RPAR,       // )

    //TERM
    nil,
    ID,

    //COMMENTS
    SINGLECOM,   // # -SINGLE-LINE COMMENTS
    BEGCOM,      // =begin - BLOCK COMMENTS BEGIN
    ENCOM,       // =end   - BLOCK COMMENTS END


