//
// Created by Ancient on 25. 11. 2018.
//
#include <stdio.h>
#include "expressions.h"
#include "scanner.h"
#include "errors.h"
#include "sym_stack.h"
#define TABLE_SIZE 7

S_stack stack;
typedef enum prec_table_relations
{
    R, // > reduce
    S, // < shift
    X, //
    E // =
}prec_table_relations_enum;

typedef enum prec_table_index
{
    i_plus_minus, // +,-
    i_mul_div,    // *,/
    i_rel_op,     // == != <= < >= >
    i_left_par,      // (
    i_right_par,      // )
    i_data,       // i (id, int, float, string)
    i_dollar      // $
}Prec_table_index_enum;

int prec_table[TABLE_SIZE][TABLE_SIZE] =
        {
                //	| +-| */ | r | ( | ) | i | $ |
                { R , S  , R , S , R , S , R }, /// +-
                { R , R  , R , S , R , S , R }, /// */
                { S , S  , X , S , R , S , R }, /// r  = <> <= < >= >
                { S , S  , S , S , E , S , X }, /// (
                { R , R  , R , X , R , X , R }, /// )
                { R , R  , R , X , R , X , R }, /// i (id, int, float, string)
                { S , S  , S , S , X , S , X }  /// $
        };

static Prec_table_index_enum get_prec_table_index (Prec_table_symbols_enum symbol){
    switch(symbol){
        case P_PLUS:
        case P_MINUS:
            return i_plus_minus;

        case P_MUL:
        case P_DIV:
            return i_mul_div;

        case P_EQ:     // ==
        case P_NOT_EQ: // !=
        case P_LESS_EQ:// <=
        case P_MORE_EQ: // >=
        case P_LESS:   // <
        case P_MORE:   // >
            return i_rel_op;

        case P_LEFT_PAR:
            return i_left_par;

        case P_RIGHT_PAR:
            return i_right_par;

        case P_ID:
        case P_INT_NUM:
        case P_FLOAT_NUM:
        case P_STRING:
            return i_data;

        default:
            return i_dollar;

    }

}

static Prec_table_symbols_enum token_to_symbol (tToken *token){
    switch(token->type){
        case PLUS:
            return P_PLUS;
        case MINUS:
            return P_MINUS;
        case MULT:
            return P_MUL;
        case DIVIDE:
            return P_DIV;
        case EQUAL:
            return P_EQ;
        case NOTEQUAL:
            return P_NOT_EQ;
        case LOE:
            return P_LESS_EQ;
        case MOE:
            return P_MORE_EQ;
        case MORE:
            return P_MORE;
        case LESS:
            return P_LESS;
        case LPAR:
            return P_LEFT_PAR;
        case RPAR:
            return P_RIGHT_PAR;
        case IDENTIFICATOR:
            return P_ID;
        case INT:
            return P_INT_NUM;
        case FLOAT:
            return P_FLOAT_NUM;
        case STRING:
            return P_STRING;
        default:
            return P_DOLLAR;
    }

}
