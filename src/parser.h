//
// Created by Dominik Vagala on 18. 11. 2018.
//

#include "scanner.h"
#include "stdbool.h"

int error_code;

#ifndef IFJ_PROJECT_PARSER_H
#define IFJ_PROJECT_PARSER_H

#endif //IFJ_PROJECT_PARSER_H


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
        "SYMBOLS" "LBRA",       // { 111
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

tToken analyze_expression(tToken token, tToken aheadToken);
