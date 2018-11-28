//
// Created by Ancient on 25. 11. 2018.
//
#include <stdio.h>
#include <malloc.h>
#include "expressions.h"
#include "errors.h"
#include "sym_stack.h"
#include "scanner.h"
#include "postfix_data_managment.h"

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


ReturnData release_resources(int error_code, S_stack stack, Bnode *tree, Output_queue *q, Operator_stack operator_stack, ReturnData data){
    data.error=true;
    data.error_code = error_code;
    s_free(&stack);
    free_symtable(tree);
    queue_dispose(q);
    operator_stack_free(&operator_stack);
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
//    else{
//        if((error=semantics(rule,count,o1,o2,o3,&final_type))) return error;
//    }

    stack_n_pop(&stack, count +1);
    s_push(&stack, P_NON_TERM, type_undefined);
    return SYNTAX_OK;

}

bool is_token_an_operator(tToken *token){
    if(token_to_symbol(*token) < P_LEFT_PAR)
        return true;
    return false;
}

bool is_token_end_symbol(tToken *token){
    switch (token->type){
        case THEN:
        case DO:
        case EOL_CASE:
            return true;
        default: return false;
    }
}

int generate_postfix(tToken *token, Operator_stack stack, Output_queue *q){

    if(!is_token_end_symbol(token)) {
        if (get_type_from_token(token) == type_float || get_type_from_token(token) == type_integer || get_type_from_token(token) == type_string || token->type == IDENTIFICATOR ){
            if (!determine_type_and_insert(q, token))
                return INNER_ERROR;
        }

        if (is_token_an_operator(token)) {
            if (stack.top != NULL) {
                prec_table_relations_enum relation = prec_table[get_prec_table_index(
                        stack.top->operator)][get_prec_table_index(token_to_symbol(*token))];
                while (stack.top != NULL &&(relation == R || relation == E) && stack.top->operator != P_LEFT_PAR) {
                    pop_to_output_queue(&stack, q);
                }
                if (!operator_stack_push(&stack, token_to_symbol(*token)))
                    return INNER_ERROR;
            }
        }
        if (token_to_symbol(*token) == P_LEFT_PAR) {
            if (!operator_stack_push(&stack, token_to_symbol(*token)))
                return INNER_ERROR;
        }
        if (token_to_symbol(*token) == P_RIGHT_PAR) {
            while (stack.top->operator != P_LEFT_PAR) {
                pop_to_output_queue(&stack, q);
            }
            operator_pop(&stack);
        }
    }
    //if there are no more tokens to read, we pop all the operators from the stack to output queue
    else{
        while(stack.top != NULL){
            pop_to_output_queue(&stack, q);
        }
    }
    return SYNTAX_OK;

}


ReturnData analyze_expresssion(tToken token, tToken aheadToken, bool tokenLookAheadFlag, Bnode *tree ){
    ReturnData *data = malloc(sizeof(ReturnData));
    data->error = false;
    int error;
    S_stack stack = s_init();

    Output_queue q = queue_inint();
    Operator_stack o_stack = operator_stack_init();

    if(!s_push(&stack,P_DOLLAR,type_undefined))
        return release_resources(INNER_ERROR, stack, tree, &q, o_stack, *data);

    Prec_table_symbols_enum stack_top_terminal_symbol = get_top_terminal(&stack)->symbol;
    data->token = token;
    Prec_table_symbols_enum new_symbol = token_to_symbol(token);

    bool enough = false;
    while(!enough){
        switch(prec_table[get_prec_table_index(stack_top_terminal_symbol)][get_prec_table_index(new_symbol)]){
            case R:
                if((error = rule_reduction(data, stack))){
                    release_resources(error, stack, tree, &q ,o_stack, *data);
                }
                break;
            case S:
                if(!insert_after_top_terminal(&stack,P_STOP,type_undefined))
                    return release_resources(INNER_ERROR, stack, tree, &q ,o_stack, *data);

                if(!s_push(&stack, new_symbol, get_type_from_token(&data->token)))
                    return release_resources(INNER_ERROR, stack, tree, &q, o_stack, *data);

                if(new_symbol == P_ID){
                    if(!is_variable_defined(tree,token.data.string))
                        return release_resources(SEMANTIC_ERROR,stack,tree, &q, o_stack, *data);
                }
//
//              if( new_symbol == P_ID || new_symbol == P_INT_NUM || new_symbol == P_FLOAT_NUM || new_symbol == P_STRING )
//                    //TODO PUSHS value of token
                      //

                if(tokenLookAheadFlag){
                    data->token = aheadToken;
                    error = generate_postfix(&data->token, o_stack, &q);
                    if(error != 0)
                        return release_resources(error, stack, tree, &q, o_stack, *data);
                    tokenLookAheadFlag = false;
                }
                else {
                    data->token = nextToken();
                    error = generate_postfix(&data->token, o_stack, &q);
                    if(error != 0)
                        return release_resources(error, stack, tree, &q, o_stack, *data);
                }
                new_symbol = token_to_symbol(data->token);
                break;
            case X:
                if ((data->token.type == EOL_CASE || data->token.type == THEN || data->token.type == DO) && stack_top_terminal_symbol == P_STOP)
                    enough = true;
                else return release_resources(SYNTAX_ERROR, stack, tree, &q, o_stack, *data);
                break;
            case E:
                if(!s_push(&stack,new_symbol,get_type_from_token(&data->token)))
                    return release_resources(INNER_ERROR, stack, tree, &q, o_stack, *data);//malloc chceck
                break;
            default: break;
        }

    }

   return release_resources(SYNTAX_OK, stack, tree, &q , o_stack, *data);
};

//int main(){
//    printf("%s",nextToken().data.string);
//}