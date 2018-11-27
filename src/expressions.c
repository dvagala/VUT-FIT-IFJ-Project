//
// Created by Ancient on 25. 11. 2018.
//
#include <stdio.h>
#include <malloc.h>
#include "expressions.h"
#include "errors.h"
#include "sym_stack.h"
#define TABLE_SIZE 7
#define SYNTAX_OK 0

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

static Prec_table_symbols_enum token_to_symbol (tToken token){
    switch(token.type){
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


ReturnData release_resources(int error_code, S_stack stack, Bnode *tree, ReturnData data){
    data.error=true;
    data.error_code = error_code;
    s_free(&stack);
    free_symtable(tree);
    return data;

}
Expr_rules_enum test_rule(int count, S_item *o1, S_item *o2, S_item *o3){

    if (count == 1){
        if (o1->symbol == INT || o1->symbol == FLOAT || o1->symbol == STRING || o1->symbol == IDENTIFICATOR )
            return operand;//E -> i
        else return no_rule;
    }
    if(count == 3){
        if(o1->symbol == P_NON_TERM && o2->symbol == P_PLUS && o3->symbol == P_NON_TERM)
            return E_plus_E; // E -> E + E

        else if(o1->symbol == P_NON_TERM && o2->symbol == P_MINUS && o3->symbol == P_NON_TERM)
            return E_minus_E; // E -> E - E

        else if(o1->symbol == P_NON_TERM && o2->symbol == P_MUL && o3->symbol == P_NON_TERM)
            return E_mul_E; // E -> E * E

        else if(o1->symbol == P_NON_TERM && o2->symbol == P_DIV && o3->symbol == P_NON_TERM)
            return E_div_E; // E -> E / E

        else if(o1->symbol == P_NON_TERM && o2->symbol == P_EQ && o3->symbol == P_NON_TERM)
            return E_eq_E; // E -> E == E

        else if(o1->symbol == P_NON_TERM && o2->symbol == P_NOT_EQ && o3->symbol == P_NON_TERM)
            return E_not_eq_E; // E -> E != E

        else if(o1->symbol == P_NON_TERM && o2->symbol == P_LESS && o3->symbol == P_NON_TERM)
            return E_less_E; // E -> E < E

        else if(o1->symbol == P_NON_TERM && o2->symbol == P_MORE && o3->symbol == P_NON_TERM)
            return E_more_E; // E -> E > E

        else if(o1->symbol == P_NON_TERM && o2->symbol == P_LESS_EQ && o3->symbol == P_NON_TERM)
            return E_less_eq_E; // E -> E <= E

        else if(o1->symbol == P_NON_TERM && o2->symbol == P_MORE_EQ && o3->symbol == P_NON_TERM)
            return E_more_eq_E; // E -> E >= E

        else if(o1->symbol == P_LEFT_PAR && o2->symbol == P_NON_TERM && o3->symbol == P_RIGHT_PAR)
            return lpar_E_rpar; // E -> (E)


    }
    return no_rule;
}

int semantics(int count, Expr_rules_enum rule, S_item *o1, S_item *o2, S_item *o3, Data_type *final_type){
    bool o1_to_float = false;
    bool o3_to_float = false;
    switch(rule){
        case operand:
            return o1->data_type;
        case lpar_E_rpar:
            return o2->data_type;
        case E_plus_E:
        case E_minus_E:
        case E_mul_E:
            if(o1->data_type == type_integer && o3->data_type == type_integer) {
                *final_type = type_integer; //int + int
                break;
            }
            if(o1->data_type == type_string && rule == E_plus_E && o3->data_type == type_string) {
                *final_type = type_string;//string concat
                break;
            }
            if(o1->data_type == type_string || o3->data_type == type_string) {
                return COMPATIBILITY_ERROR; //string + not_a_string
            }
            *final_type = type_float;
            if(o1->data_type == type_float && o3->data_type == type_integer)
                o3_to_float = true;

            if(o3->data_type == type_float && o1->data_type == type_integer)
                o1_to_float = true;
            break;

        case E_div_E:
            *final_type = type_float;

            if(o1->data_type == type_float && o3->data_type == type_integer)
                o3_to_float = true;

            if(o3->data_type == type_float && o1->data_type == type_integer)
                o1_to_float = true;

            if(o1->data_type == type_string || o3->data_type == type_string)
                return COMPATIBILITY_ERROR;
            break;

        case E_eq_E:
        case E_not_eq_E:
        case E_more_E:
        case E_less_E:
        case E_less_eq_E:
        case E_more_eq_E:
            *final_type = type_boolean;

            if(o1->data_type == type_float && o3->data_type == type_integer) {
                o3_to_float = true;
                break;
            }

            if(o3->data_type == type_float && o1->data_type == type_integer) {
                o1_to_float = true;
                break;
            }
            if(o1->data_type != o3->data_type)
                return COMPATIBILITY_ERROR;


        default: break;
    }

//    if(o1_to_float); //TODO generate code
//    if(o3_to_float); //TODO generate code
    return SYNTAX_OK;
}

int rule_reduction(ReturnData *data, S_stack stack){
    bool stop = false;
    int count = get_count_after_stop(&stack, &stop);
    int error;
    S_item *o1 = NULL;
    S_item *o2 = NULL;
    S_item *o3 = NULL;
    Expr_rules_enum rule;
    Data_type final_type;
    if (count == 1 && stop){
        o1 = stack.top;
        rule = test_rule(count,o1,o2,o3);
    }
    else if (count == 3 && stop){
            o1 = stack.top;
            o2 = stack.top->next_item;
            o3 = stack.top->next_item->next_item;
            rule = test_rule(count, o1, o2, o3);
    }
    else return SYNTAX_ERROR;

    if (rule == no_rule){
        return SYNTAX_ERROR;
    }
    else{
        if((error=semantics(rule,count,o1,o2,o3,&final_type))) return error;
    }

    //TODO generate code

    stack_n_pop(&stack, count +1);
    s_push(&stack, P_NON_TERM, final_type);
    return SYNTAX_OK;

}


ReturnData analyze_expresssion(tToken token, tToken aheadToken, bool tokenLookAheadFlag, Bnode *tree){
    ReturnData *data = malloc(sizeof(ReturnData));
    data->error = false;
    int error;
    S_stack stack;
    s_init(&stack);

    if(!s_push(&stack,P_DOLLAR,type_undefined))
        return release_resources(INNER_ERROR, stack, tree, *data);

    Prec_table_symbols_enum stack_top_terminal_symbol = get_top_terminal(&stack)->symbol;
    data->token = token;
    Prec_table_symbols_enum new_symbol = token_to_symbol(token);

    bool enough = false;
    while(!enough){
        switch(prec_table[stack_top_terminal_symbol][new_symbol]){
            case R:
                if((error = rule_reduction(data, stack))){
                    release_resources(error, stack, tree, *data);
                }
                break;
            case S:
                if(!insert_after_top_terminal(&stack,P_STOP,type_undefined))
                    return release_resources(INNER_ERROR, stack, tree, *data);

                if(!s_push(&stack,P_STOP,type_undefined))
                    return release_resources(INNER_ERROR, stack, tree, *data);

//                if( new_symbol == INT || new_symbol == FLOAT || new_symbol == STRING || new_symbol == IDENTIFICATOR )
//                    //TODO generate code
                if(tokenLookAheadFlag){
                    data->token = aheadToken;
                    tokenLookAheadFlag = false;
                }
                else data->token = nextToken();
                new_symbol = token_to_symbol(data->token);
                break;
            case X:
                if ((data->token.type == EOL_CASE || data->token.type == THEN || data->token.type == DO) && stack_top_terminal_symbol == P_STOP)
                    enough = true;
                else return release_resources(SYNTAX_ERROR, stack, tree, *data);
                break;
            case E:
                if(!s_push(&stack,new_symbol,get_type_from_token(&data->token)))
                    return release_resources(INNER_ERROR, stack, tree, *data);//malloc chceck
                break;
            default: break;
        }

    }


};