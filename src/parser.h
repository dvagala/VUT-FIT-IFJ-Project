//
// Created by Dominik Vagala on 18. 11. 2018.
//

#ifndef IFJ_PROJECT_PARSER_H
#define IFJ_PROJECT_PARSER_H

#include "scanner.h"
#include "symtable.h"



int error_code;

const char* token_type_enum_string[] = {

        "IF", "ELSE", "DEF", "DO", "END", "NOT", "NIL", "THEN", "WHILE", //012345678
        [20] = "WORD", "NUMBER", "EQUALCASES", "LESSCASES", "MORECASES", "KEYWORDCASES", //20,21,22,23,24,25
        [500] = "EOL_CASE", "EOF_CASE", "ERROR", //500, 501, 502
        [1000] = "INT", "FLOAT", "STRING", "FLOAT_EXPO", "IDENTIFICATOR", //1000, 1001, 1002, 1003, 1004
        [100] = "PLUS", // + 100
        "MINUS",      // - 101
        "MULT",       // * 102
        "DIVIDE",     // / 103
        "ASSIGN",     // = 104
        "LESS",       // < 105
        "MORE",       // > 106
        "LOE",        // <=107
        "MOE",        // >=108
        "EQUAL",      // ==109
        "NOTEQUAL",   // !=110
        "LBRA",       // { 111
        "RBRA",       // } 112
        "COLON",      // , 113
        "LPAR",       // ( 114
        "RPAR",       // ) 115
        [200] = "SINGLECOM",   // # -"SINGLE""LINE"" "COMMENTS"  200
        "BEGCOM",      // =begin - "BLOCK""COMMENTS""BEGIN"201
        "EXPR"     // Add-on for parser for cleaner code, in scanner is not used 202
};


bool state();
bool expr();
bool term();
bool param();
bool more_param();
bool st_list();

//tToken analyze_expression(tToken token, tToken aheadToken, bool lookahead_occured);


#define LABEL "LABEL"
#define JUMP "JUMP"
#define JUMPIFNEQ "JUMPIFNEQ"
#define EXIT "EXIT"
#define JUMPIFEQS "JUMPIFEQS"
#define JUMPIFNEQS "JUMPIFNEQS"
#define JUMPIFEQ "JUMPIFEQ"
#define CREATEFRAME "CREATEFRAME"
#define PUSHFRAME "PUSHFRAME"
#define POPFRAME "POPFRAME"
#define MOVE "MOVE"
#define BREAK "BREAK"
#define DPRINT "DPRINT"
#define READ "READ"
#define WRITE "WRITE"
#define CONCAT "CONCAT"
#define STRLEN "STRLEN"
#define GETCHARSETCHAR "GETCHARSETCHAR"
#define TYPE "TYPE"
#define INT2FLOATS "INT2FLOATS"
#define FLOAT2INTS "FLOAT2INTS"
#define INT2CHARS "INT2CHARS"
#define STRI2INTS "STRI2INTS"
#define ANDS "ANDS"
#define ORS "ORS"
#define STRI2INT "STRI2INT"
#define INT2FLOAT "INT2FLOAT"
#define INT2CHAR "INT2CHAR"
#define FLOAT2INT "FLOAT2INT"
#define NOTS "NOTS"
#define LTS "LTS"
#define GTS "GTS"
#define AND "AND"
#define OR "OR"
#define NOT "NOT"
#define EQS "EQS"
#define LT "LT"
#define GT "GT"
#define EQ "EQ"
#define ADDS "ADDS"
#define SUBS "SUBS"
#define MULS "MULS"
#define DIVS "DIVS"
#define IDIVS "IDIVS"
#define DEFVAR "DEFVAR"
#define CALL "CALL"
#define IDIV "IDIV"
#define DIV "DIV"
#define MUL "MUL"
#define SUB "SUB"
#define ADD "ADD"
#define CLEARS "CLEARS"
#define RETURN "RETURN"
#define POPS "POPS"
#define PUSHS "PUSHS"




#endif //IFJ_PROJECT_PARSER_H