//
// Created by Ancient on 25. 11. 2018.
//
#include <stdio.h>
#include <malloc.h>
#include "expressions.h"


#define TABLE_SIZE 7
#define SYNTAX_OK 0

#define DEBUG_EXPRESSION_ANALYSIS 0         // Set to '1', if you want to print debug stuff

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
                { S , S  , X , S , R , S , R }, /// r  == != <= < >= >
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
bool is_token_end_symbol(tToken *token){
    switch (token->type){
        case THEN:
        case DO:
        case EOL_CASE:
        case EOF_CASE:
            return true;
        default: return false;
    }
}


bool is_token_an_operator(tToken *token){
    if(token_to_symbol(*token) < P_LEFT_PAR)
        return true;
    return false;
}


ReturnData release_resources(int error_code, S_stack stack, Bnode tree, Output_queue *q, Operator_stack operator_stack, ReturnData data){
    data.error=true;
    data.error_code = error_code;
    s_free(&stack);
    if(tree)free_symtable(&tree);
    queue_dispose(q);
    operator_stack_free(&operator_stack);
    return data;

}

ReturnData release_resources_and_success(int error_code, S_stack stack, Bnode tree, Output_queue *q, Operator_stack operator_stack, ReturnData data){
    data.error=false;
    data.error_code = error_code;
    s_free(&stack);
    if(tree)free_symtable(&tree);
    queue_dispose(q);
    operator_stack_free(&operator_stack);
    return data;

}
int generate_postfix(tToken *token, Operator_stack *stack, Output_queue *q){
    bool par_found = false;
    if(!is_token_end_symbol(token)) {
        if (get_type_from_token(token) == type_float || get_type_from_token(token) == type_integer ||
            get_type_from_token(token) == type_string || token->type == IDENTIFICATOR) {
            if (!determine_type_and_insert(q, token))
                return INNER_ERROR;
            else if(DEBUG_EXPRESSION_ANALYSIS) printf("EXPRESSIONS: %s\n", "added operand to queue");
        }

        if (is_token_an_operator(token)) {
            if(stack->top != NULL) {
                prec_table_relations_enum relation = prec_table[get_prec_table_index(
                        stack->top->operator)][get_prec_table_index(token_to_symbol(*token))];
                while (stack->top != NULL && (relation == R || relation == E) && stack->top->operator != P_LEFT_PAR) {
                    pop_to_output_queue(stack, q);
                }
                if (!operator_stack_push(stack, token_to_symbol(*token)))
                    return INNER_ERROR;

                else if(DEBUG_EXPRESSION_ANALYSIS) printf("EXPRESSIONS: %s\n", "added operator to stack");
            }
            else if (!operator_stack_push(stack, token_to_symbol(*token))) {
                return INNER_ERROR;
            } else {
                if(DEBUG_EXPRESSION_ANALYSIS) printf("EXPRESSIONS: %s\n", "added operator to queue");

            }
        }

        if (token_to_symbol(*token) == P_LEFT_PAR) {
            if (!operator_stack_push(stack, token_to_symbol(*token)))
                return INNER_ERROR;
        }
        if (token_to_symbol(*token) == P_RIGHT_PAR) {
            while (stack->top->operator != P_LEFT_PAR ) {
                if(stack->top==NULL)
                    return SYNTAX_ERROR;
                pop_to_output_queue(stack, q);

            }
            if(stack->top==NULL)
                return SYNTAX_ERROR;
            operator_pop(stack);
        }
    }
    else{    //if there are no more tokens to read, we pop all the operators from the stack to output queue
        while(stack->top!=NULL){
            pop_to_output_queue(stack, q);
        }

    }

    return SYNTAX_OK;

}

Expr_rules_enum test_rule(int count, S_item *o1, S_item *o2, S_item *o3){

    if (count == 1){
        if (o1->symbol == P_INT_NUM|| o1->symbol == P_FLOAT_NUM || o1->symbol == P_STRING || o1->symbol == P_ID )
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

int rule_reduction( S_stack *stack){
    int count = get_count_after_stop(*stack);
    S_item *o1 = NULL;
    S_item *o2 = NULL;
    S_item *o3 = NULL;
    Expr_rules_enum rule;

    if(DEBUG_EXPRESSION_ANALYSIS) printf("EXPRESSIONS: %s%d\n","count is: ",count);
    if (count == 0){
        return SYNTAX_ERROR;
    }
    if (count == 1){
        o1 = stack->top;
        rule = test_rule(count,o1,o2,o3);
    }
    else if (count == 3 ){
            o3 = stack->top;
            o2 = stack->top->next_item;
            o1 = stack->top->next_item->next_item;
            rule = test_rule(count, o1, o2, o3);
    }
    else return SYNTAX_ERROR;

    if (rule == no_rule){
        return SYNTAX_ERROR;
    }

    if(DEBUG_EXPRESSION_ANALYSIS) printf("EXPRESSIONS: %s%d\n","the rule is: ",rule);
    stack_n_pop(stack, count +1);
    s_push(stack, P_NON_TERM);

    if(DEBUG_EXPRESSION_ANALYSIS) printf("EXPRESSIONS: %s%d\n","top stack is: ",stack->top->symbol);

    if(DEBUG_EXPRESSION_ANALYSIS) printf("EXPRESSIONS: %s%d\n","top terminal is: ",get_top_terminal(stack)->symbol);

    return SYNTAX_OK;

}

ReturnData analyze_expresssion(tToken token, tToken aheadToken, bool tokenLookAheadFlag, Bnode *tree ){
    //allocating data managment
    ReturnData *data = malloc(sizeof(ReturnData));
    data->error = false;
    int error;
    S_stack *stack =  malloc(sizeof(S_stack));
    s_init(stack);
    Output_queue *q = (Output_queue*)malloc(sizeof(Output_queue)) ;
    queue_inint(q);
    Operator_stack *o_stack = malloc(sizeof(Operator_stack));
    operator_stack_init(o_stack);


    if(!s_push(stack,P_DOLLAR))
        return release_resources(INNER_ERROR, *stack, *tree, q, *o_stack, *data);

    Prec_table_symbols_enum *stack_top_terminal_symbol;
    data->token = &token;
    Prec_table_symbols_enum new_symbol = token_to_symbol(token);
    error = generate_postfix(data->token, o_stack, q);
    if(error != 0)
        return release_resources(error, *stack, *tree, q, *o_stack, *data);

    bool enough = false;
    while(!enough){

        if(DEBUG_EXPRESSION_ANALYSIS) printf("EXPRESSIONS: %s%d\n","next token is: ",new_symbol);
        stack_top_terminal_symbol = &get_top_terminal(stack)->symbol;
        if(new_symbol == P_ID && !is_token_end_symbol(&token)){
            if(!is_variable_defined(tree,token.data.string))
                return release_resources(SEMANTIC_ERROR,*stack,*tree, q, *o_stack, *data);
        }

        if(stack->top == NULL)
            return release_resources(INNER_ERROR, *stack, *tree, q ,*o_stack, *data);
        switch(prec_table[get_prec_table_index(*stack_top_terminal_symbol)][get_prec_table_index(new_symbol)]){
            case R:
                if(DEBUG_EXPRESSION_ANALYSIS) printf("EXPRESSIONS: %s\n","I'm in R!");
                if((error = rule_reduction(stack))){
                    return release_resources(error, *stack, *tree, q ,*o_stack, *data);
                }
                break;
            case S:
                if(DEBUG_EXPRESSION_ANALYSIS) printf("EXPRESSIONS: %s\n","I'm in S!");
                if(!insert_after_top_terminal(stack,P_STOP))
                    return release_resources(INNER_ERROR, *stack, *tree, q ,*o_stack, *data);

                if(!s_push(stack, new_symbol))
                    return release_resources(INNER_ERROR, *stack, *tree, q, *o_stack, *data);

                if(new_symbol == P_ID){
                    if(!is_variable_defined(tree,token.data.string))
                        return release_resources(SEMANTIC_ERROR,*stack,*tree, q, *o_stack, *data);
                }
//
//              if( new_symbol == P_ID || new_symbol == P_INT_NUM || new_symbol == P_FLOAT_NUM || new_symbol == P_STRING )


                if(tokenLookAheadFlag){
                    data->token = &aheadToken;
                    error = generate_postfix(data->token, o_stack, q);
                    if(error != 0)
                        return release_resources(error, *stack, *tree, q, *o_stack, *data);
                    tokenLookAheadFlag = false;
                }
                else {
                    *data->token = nextToken();
                    error = generate_postfix(data->token, o_stack, q);
                    if(error != 0)
                        return release_resources(error, *stack, *tree, q, *o_stack, *data);
                }
                new_symbol = token_to_symbol(*data->token);

                break;
            case X:
                if(DEBUG_EXPRESSION_ANALYSIS) printf("EXPRESSIONS: %s\n","I'm in X!");
                if (is_token_end_symbol(data->token) && *stack_top_terminal_symbol == P_DOLLAR) {
                    if(DEBUG_EXPRESSION_ANALYSIS) printf("EXPRESSIONS: %s\n","ENOUGH!, very nice");
                    enough = true;
                }
                else return release_resources(SYNTAX_ERROR, *stack, *tree, q, *o_stack, *data);
                break;
            case E:
                if(DEBUG_EXPRESSION_ANALYSIS) printf("EXPRESSIONS: %s\n","I'm in E!");
                if(!s_push(stack,new_symbol))
                    return release_resources(INNER_ERROR, *stack, *tree, q, *o_stack, *data);
                *data->token = nextToken();
                new_symbol = token_to_symbol(*data->token);
                error = generate_postfix(data->token, o_stack, q);
                if(error != 0)
                    return release_resources(error, *stack, *tree, q, *o_stack, *data);

                break;
            default: break;
        }

    }
    print_queue(*q);
   return release_resources_and_success(SYNTAX_OK, *stack, NULL, q , *o_stack, *data);
};

//int main(){
//    tToken token = nextToken();
//    Bnode tree;
//    local_symtable_init(&tree);
//
//
//    add_variable_to_symtable(&tree,"a");
//    add_variable_to_symtable(&tree,"b");
//    add_variable_to_symtable(&tree,"c");
//    ReturnData data = analyze_expresssion(token,token,false,&tree);
//    if(DEBUG_EXPRESSION_ANALYSIS) printf("EXPRESSIONS: %s%d\n","error code:", data.error_code);
//
//
//    free_symtable(&tree);
//
//
//}
//
