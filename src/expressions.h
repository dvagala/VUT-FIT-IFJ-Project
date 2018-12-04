/*
 * Project: Implementace překladače imperativního jazyka IFJ18
 *
 * File: expressions.h
 *
 * xglosk00   Kristian Glos
 * xvagal00   Dominik Vagala
 * xvinsj00   Jakub Vins
 * xabike00   Zhamilya Abikenova
 *
 */

#ifndef IFJ_PROJECT_EXPRESSION_H
#define IFJ_PROJECT_EXPRESSION_H

#include "errors.h"
#include "sym_stack.h"
#include "postfix_data_managment.h"

bool im_in_while_loop;

typedef enum expr_rules{
               // E->
    E_plus_E, //    E + E
    E_minus_E, //   E - E
    E_mul_E,   //   E * E
    E_div_E,    //  E / E
    lpar_E_rpar,  //  E = (E)
    E_eq_E,     //  E == E
    E_not_eq_E, //  E != E
    E_less_E,   //  E < E
    E_more_E,   //  E > E
    E_less_eq_E,//  E <= E
    E_more_eq_E,//  E >= E
    operand,    //  i
    no_rule     //  no suitable rule


}Expr_rules_enum;

typedef struct {
    tToken *token;
    bool error;
    int error_code;
}ReturnData;

ReturnData *analyze_expresssion(tToken token, tToken aheadToken, bool tokenLookAheadFlag, Bnode *tree );
#endif //IFJ_PROJECT_EXPRESSION_H
