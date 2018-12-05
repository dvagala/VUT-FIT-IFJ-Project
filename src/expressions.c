/*
 * Project: Implementace překladače imperativního jazyka IFJ18
 *
 * File: expressions.c
 *
 * xglosk00   Kristian Glos
 * xvagal00   Dominik Vagala
 * xvinsj00   Jakub Vins
 * xabike00   Zhamilya Abikenova
 *
 */

#include <stdio.h>
#include <malloc.h>
#include "expressions.h"
#include "code_gen.h"
#include "scanner.h"


#define TABLE_SIZE 7
#define SYNTAX_OK 0

#define DEBUG_EXPRESSION_ANALYSIS 0        // Set to '1', if you want to print debug stuff


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

/** Returns precedence table index*/
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

/** Function that returns true, if token is possible end symbol*/
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

/** Function that returns true, if token is operator*/
bool is_token_an_operator(tToken *token){
    if(token_to_symbol(*token) < P_LEFT_PAR)
        return true;
    return false;
}

/** Function that correctly frees allocated structures and returns updated data*/
ReturnData *release_resources(int error_code, S_stack *stack, Output_queue *q, Operator_stack *operator_stack, ReturnData *data){
    if(error_code==0)
        data->error=false;
    else data->error=true;
    data->error_code = error_code;
    if(DEBUG_EXPRESSION_ANALYSIS)printf("%s%d\n","error code is:",error_code);
    s_free(stack);
    free(stack);
    queue_dispose(q);
    free(q);
    operator_stack_free(operator_stack);
    free(operator_stack);
    return data;
}

/** Function that generates postfix into a queue by the Shunting-yard algorithm*/
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
            if(stack->top == NULL)
                return SYNTAX_ERROR;
            while (stack->top->operator != P_LEFT_PAR ) {

                if(stack->top->operator == P_LEFT_PAR)
                    par_found = true;
                pop_to_output_queue(stack, q);

                if(stack->top == NULL && !par_found)
                    return SYNTAX_ERROR;
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

/** Function that tests, if any rule applies to inserted operands/operators.
 *  Returns that rule*/
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

/** function that reduces top stack operands, if any rule applies to them, if not ,returns error*/
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

/** Returns true if item is operand*/
bool is_item_operand(P_item *item){
    switch(item->operator){
        case P_STRING:
        case P_INT_NUM:
        case P_FLOAT_NUM:
        case P_ID:
            return true;
        default:
            return false;
    }
}

/** Function generates operation into the code_list based on the operator*/
int operation_gen(P_item *o2, Prec_table_symbols_enum operator){
    switch (operator) {
        case P_PLUS:
            if (DEBUG_EXPRESSION_ANALYSIS) printf("EXPRESSIONS: %s\n", "I'm adding");
            insert_simple_instruction("ADDS");
            break;
        case P_MINUS:
            insert_simple_instruction("SUBS");
            break;
        case P_MUL:
            insert_simple_instruction("MULS");
            break;
        case P_DIV:
            if (o2->operator == P_INT_NUM) {
                if (o2->res) {
                    insert_simple_instruction("DIVS");
                    return SYNTAX_OK;
                } else {
                    if (o2->value_int == 0)
                        return ZERO_ERROR;
                    insert_simple_instruction("IDIVS");
                }
            }
            else {
                if (o2->res) {
                    insert_simple_instruction("IDIVS");
                    return SYNTAX_OK;

                }
                else if (o2->value_double == 0 && !o2->res)
                    return ZERO_ERROR;
                insert_simple_instruction("DIVS");
            }
            break;
        case P_LESS:
            insert_simple_instruction("LTS");
            break;
        case P_EQ:
            insert_simple_instruction("EQS");
            break;
        case P_MORE:
            insert_simple_instruction("GTS");
            break;
        case P_NOT_EQ:
            insert_simple_instruction("EQS");
            insert_simple_instruction("NOTS");
            break;
        case P_MORE_EQ:
            insert_simple_instruction("LTS");
            insert_simple_instruction("NOTS");
            break;
        case P_LESS_EQ:
            insert_simple_instruction("GTS");
            insert_simple_instruction("NOTS");
            break;
        default:
            break;
    }
    return SYNTAX_OK;

}

/** Function generates code and checks semantics if both operands on top of the stack are undefined(variables)*/
int both_are_undefined(P_stack *post_stack, Prec_table_symbols_enum operator) {
    P_item *o2;
    P_item *o1;
    P_item *int_pom = (P_item*)malloc(sizeof(P_item));
    int_pom->operator = P_INT_NUM;
    int_pom->value_int =1;
    P_item *float_pom = (P_item*)malloc(sizeof(P_item));
    P_item *string_pom = (P_item*)malloc(sizeof(P_item));
    float_pom->operator = P_FLOAT_NUM;
    float_pom->value_double = 1;
    string_pom->operator = P_STRING;
    insert_defvar_res(im_in_while_or_if);
    declare_defvar_restype(im_in_while_or_if);

    o2 = post_stack->top;//o1 + o2
    o1 = post_stack->top->next_item;
    if(im_in_while_or_if){
        if(o1->res)
            gen_defvar_in_while(o1);
        else
            gen_defvar_in_while(o2);
    }
    else insert_instruction("DEFVAR", o2, NULL, NULL);

    insert_instruction("MOVE", o1, NULL, NULL);//MOVE Lf@%res o1

    o1->res = true;



    insert_instruction("TYPE", NULL, NULL, NULL);//TYPE LF@%res$type LF@%res
    insert_instruction("TYPE", o2, NULL, NULL);//TYPE LF@%name$type LF@name



    insert_instruction("JUMPIFEQ",o2,NULL,NULL); //JUMPIFEQ $%res$type1$LF$a LF@%res$type1 LF@a  || if they are equal
    insert_instruction("JUMPIFEQ",float_pom,NULL,NULL);// JUMPIFEQ $%res$type1$float LF@%res$type1 string@float
    insert_instruction("JUMPIFEQ",int_pom,NULL,NULL);// JUMPIFEQ $%res$type1$int LF@%res$type1 string@int
    exit_gen(4);

    insert_instruction("LABEL",float_pom,NULL,NULL); //LABEL $%res$type1$float
    insert_instruction("PUSHS",o1,NULL,NULL); // PUSHS LF@%res1
    insert_instruction("PUSHS",o2,NULL,NULL); // PUSHS LF@a
    insert_simple_instruction("INT2FLOATS");
    insert_simple_instruction("JUMP");
    gen_unique_operation(operator,float_pom); // JUMP $plus1

    insert_instruction("LABEL",int_pom,NULL,NULL); // LABEL $%res$type1$int
    insert_instruction("PUSHS",o1,NULL,NULL); //PUSHS LF@%res1
    insert_simple_instruction("INT2FLOATS");

    insert_instruction("PUSHS",o2,NULL,NULL); //PUSHS LF@a
    insert_simple_instruction("JUMP");
    gen_unique_operation(operator,float_pom);// JUMP $plus1

    insert_instruction("LABEL",o2,NULL,NULL); //LABEL $%res$type1$LF$a || o1 type == o2 type LABEL
    insert_instruction("JUMPIFEQ",string_pom,NULL,NULL); // JUMPIFEQ $%res$type1$string LF@%res$type1 string@string
    insert_instruction("PUSHS",o1,NULL,NULL);
    insert_instruction("PUSHS",o2,NULL,NULL);
    gen_custom_jumpifeq(int_pom,operator);
    gen_custom_jumpifeq(float_pom,operator);
    exit_gen(4);

    insert_instruction("LABEL",string_pom,NULL,NULL);// LABEL $%res$type1$string
    insert_instruction("CONCAT", o2, NULL,NULL);
    push_res();
    insert_instruction("JUMP",NULL,NULL,"end");//JUMP to end


    if(operator == P_DIV){
        insert_simple_instruction("LABEL");
        gen_unique_operation(operator,int_pom);// LABEL $plus1if(operator == P_DIV){
        int_pom->value_int =0;
        insert_instruction("JUMPIFEQ",o2,int_pom,"exito");
        int_pom->value_int =1;
        operation_gen(int_pom,operator);
        insert_instruction("JUMP",NULL,NULL,"end");
    }

    insert_simple_instruction("LABEL");
    gen_unique_operation(operator,float_pom);// LABEL $plus1
    if(operator== P_DIV){
        float_pom->value_double =0;
        insert_instruction("JUMPIFEQ",o2,float_pom,"exito");
        float_pom->value_double =1;
        operation_gen(float_pom,operator);
    }
    else operation_gen(o2,operator);
    insert_instruction("JUMP",NULL,NULL,"end");
    if(operator==P_DIV) {
        insert_instruction("LABEL", NULL, NULL, "exito");
        exit_gen(9);
    }
    insert_instruction("LABEL",NULL,NULL,"end");


    free(int_pom);
    free(string_pom);
    free(float_pom);

    return SYNTAX_OK;
}

/** Function generates code and checks semantics if only one of the operands on the top of the stack is undefined(variable)*/
int one_is_undefined_semantics(P_stack *post_stack,Prec_table_symbols_enum operator){
    P_item *o2;
    P_item *defined;
    P_item *non_defined ;
    P_item *o1;
    int error;
    o2 = post_stack->top;//o1 + o2
    o1 = post_stack->top->next_item;

    insert_defvar_res(im_in_while_or_if);
    declare_defvar_restype(im_in_while_or_if);
    if(o1->operator == P_ID) {
        insert_instruction("MOVE",o1, NULL,NULL);//MOVE Lf@%res o1
        insert_instruction("TYPE",NULL,NULL,NULL);//TYPE LF@%res$type LF@%res
        defined = o2;

        o1->res = true;
        non_defined = o1;

    }
    else{
        insert_instruction("MOVE",o2, NULL,NULL);//MOVE Lf@%res o2
        insert_instruction("TYPE",NULL,NULL,NULL);//TYPE LF@%res$type LF@%res
        defined = o1;
        o2->res = true;
        non_defined = o2;
    }


    if(defined->operator == P_STRING ){
        if(operator == P_PLUS){
            insert_instruction("JUMPIFEQ", defined, NULL, NULL);//pokial su rovnakeho typu
            exit_gen(4);
            insert_instruction("LABEL", defined, NULL, NULL);
            insert_instruction("CONCAT", defined, NULL,NULL);
            push_res();
            return SYNTAX_OK;
        }
        else return COMPATIBILITY_ERROR;
    }

    if(defined->operator == P_INT_NUM || defined->operator == P_FLOAT_NUM) {

        if (defined->operator == P_INT_NUM) {//pokial je int, musim kontrolovat ci res neni float
            non_defined->operator = P_FLOAT_NUM;
        }
        else non_defined->operator = P_INT_NUM;

        insert_instruction("JUMPIFEQ", non_defined, NULL, NULL);    //pokial su rozdielneho typu
        insert_instruction("PUSHS",o1,NULL,NULL);
        if(operator==P_DIV && non_defined == o2){
            P_item *pom = malloc(sizeof(P_item));
            pom->operator = defined->operator;
            pom->value_double = 0;
            pom->value_int = 0;
            insert_instruction("JUMPIFEQ",pom,NULL,"exito");
            free(pom);

        }
        insert_instruction("PUSHS",o2,NULL,NULL);
        insert_instruction("JUMPIFEQ", defined, NULL, NULL);//pokial su rovnakeho typu
        exit_gen(4);


        insert_instruction("LABEL",non_defined,NULL,NULL);
        if((o2 == non_defined && o1->operator == P_INT_NUM)||(o1==non_defined && o2->operator == P_FLOAT_NUM)){
            insert_instruction("PUSHS",o1,NULL,NULL);
            insert_simple_instruction("INT2FLOATS");
            insert_instruction("PUSHS",o2,NULL,NULL);
        }
        else{
            insert_instruction("PUSHS",o1,NULL,NULL);
            insert_instruction("PUSHS",o2,NULL,NULL);
            insert_simple_instruction("INT2FLOATS");

        }
        if(operator == P_DIV && defined->operator!=P_FLOAT_NUM){
            non_defined->value_double=0;
            insert_instruction("JUMPIFEQ",non_defined,NULL,"exito");
            insert_simple_instruction("DIVS");
            insert_instruction("JUMP",NULL,NULL,"end");
        }

        insert_instruction("LABEL", defined, NULL, NULL);
    }

    error=operation_gen(o2,operator);
    if(operator == P_DIV){
        if(non_defined==o2){
            insert_instruction("JUMP",NULL,NULL,"end");
            insert_instruction("LABEL",NULL,NULL,"exito");
            exit_gen(9);
        }
        insert_instruction("LABEL",NULL,NULL,"end");
    }


    return error;
}

/**Function decides which kind of semantics we have to call, if none of the operands is variable, we proceed to
 * check semantics and generate code in this function*/
int semantics(P_stack *stack,Prec_table_symbols_enum operator){


    P_item *o2= stack->top;
    P_item *o1 = stack->top->next_item;
    if((o1->operator == P_ID && o2->operator != P_ID) || (o2->operator == P_ID && o1->operator != P_ID))
        return one_is_undefined_semantics(stack,operator);
    if(o1->operator == P_ID && o2->operator == P_ID)
        return both_are_undefined(stack,operator);
    if (o1->operator != P_ID && o2->operator != P_ID){
        if (o1->operator == P_INT_NUM && o2->operator == P_FLOAT_NUM) {
            o1->value_double = (double) o1->value_int;
            o1->operator = P_FLOAT_NUM;

        } else if (o2->operator == P_INT_NUM && o1->operator == P_FLOAT_NUM) {
            o2->value_double = (double) o2->value_int;
            o2->operator = P_FLOAT_NUM;

        } else if (o1->operator != o2->operator) return COMPATIBILITY_ERROR;

        insert_defvar_res(im_in_while_or_if);

        if (o1->operator == P_STRING && o2->operator == P_STRING) {
            insert_instruction("CONCAT", o1, o2,NULL);
            push_res();
        } else {
            insert_instruction("PUSHS", o1, NULL,NULL);
            insert_instruction("PUSHS", o2, NULL,NULL);


            return operation_gen(o2,operator);
        }

    }

    return SYNTAX_OK;

}

/** This function evaluates postfix queue, generates ifj18code and checks semantics, has all the control over P_stack*/
int queue_evaluation(Output_queue *q){

    int error;
    char *value;
    P_stack *stack = malloc(sizeof(P_stack));
    p_stack_init(stack);
    while(q->first){
        if(is_item_operand(q->first)){

            if(DEBUG_EXPRESSION_ANALYSIS)printf("%s%d\n","EXPRESSIONS: first in queue is:",q->first->operator);

            first_from_queue_to_stack(q,stack);
            if(q->first==NULL && stack->top->next_item==NULL){
                insert_instruction("PUSHS",stack->top,NULL,NULL);
                p_stack_pop(stack);
                free(stack);
                return SYNTAX_OK;
            }

        }
        else if(q->first->operator < P_LEFT_PAR){//operator
            value = malloc(sizeof(int)*2+1);
            error =semantics( stack, q->first->operator);
            insert_simple_instruction("#--------------------------------------------------------");
            if(error!=SYNTAX_OK) {
                free(value);
                p_stack_free(stack);
                free(stack);
                return error;
            }
            delete_first(q);
            if(q->first==NULL) {
                generic_label_count++;
                p_stack_free(stack);
                free(stack);
                free(value);
                return SYNTAX_OK;
            }
            p_stack_pop(stack);
            p_stack_pop(stack);
            insert_instruction("POPS",NULL,NULL,NULL);
            strcpy(value,"%res");
            sprintf(value,"%s%d",value,generic_label_count);
            p_stack_push(stack,true,0,0,value,P_ID); //inserts result from last calculation to stack top

            generic_label_count++;
            free(value);
        }
        else{
            p_stack_free(stack);
            free(stack);
            return SYNTAX_ERROR;
        }
        if(DEBUG_EXPRESSION_ANALYSIS) printf("%s%d\n","q first is:",q->first->operator);
    }

    if(DEBUG_EXPRESSION_ANALYSIS) printf("%s\n","EXPRESSIONS: Queue eval done");
    p_stack_free(stack);
    free(stack);
    return SYNTAX_OK;
}

/** Main function of expression analysis, calls all the other functions and performs syntax analysis*/
ReturnData *analyze_expresssion(tToken token, tToken aheadToken, bool tokenLookAheadFlag, Bnode *tree ){
    //allocating data managment
    ReturnData *data = (ReturnData*)malloc(sizeof(ReturnData));
    data->error = false;
    int error = SYNTAX_OK;
    S_stack *stack = (S_stack*)malloc(sizeof(S_stack));
    s_init(stack);
    Output_queue *q = malloc(sizeof(Output_queue)) ;
    if(DEBUG_EXPRESSION_ANALYSIS) printf("%s%lu\n","size of output queue:",sizeof(Output_queue));
    queue_inint(q);
    Operator_stack *o_stack = malloc(sizeof(Operator_stack));
    operator_stack_init(o_stack);


    if(!s_push(stack,P_DOLLAR))
        return release_resources(INNER_ERROR, stack, q, o_stack, data);

    Prec_table_symbols_enum *stack_top_terminal_symbol;
    data->token = &token;
    Prec_table_symbols_enum new_symbol = token_to_symbol(token);
    error = generate_postfix(data->token, o_stack, q);
    if(error != 0)
        return release_resources(error, stack, q, o_stack, data);

    bool enough = false;
    while(!enough){

        if(DEBUG_EXPRESSION_ANALYSIS) printf("EXPRESSIONS: %s%d\n","current token is: ",data->token->type);
        stack_top_terminal_symbol = &get_top_terminal(stack)->symbol;
        if(new_symbol == P_ID && !is_token_end_symbol(&token)){
            if(!is_variable_defined(tree,data->token->data.string))
                return release_resources(SEMANTIC_ERROR,stack, q,o_stack, data);
        }

        if(stack->top == NULL)
            return release_resources(INNER_ERROR, stack, q ,o_stack, data);
        switch(prec_table[get_prec_table_index(*stack_top_terminal_symbol)][get_prec_table_index(new_symbol)]){
            case R:
                if(DEBUG_EXPRESSION_ANALYSIS) printf("EXPRESSIONS: %s\n","I'm in R!");
                if((error = rule_reduction(stack))){
                    return release_resources(error, stack,  q ,o_stack, data);
                }
                break;
            case S:
                if(DEBUG_EXPRESSION_ANALYSIS) printf("EXPRESSIONS: %s\n","I'm in S!");
                if(!insert_after_top_terminal(stack,P_STOP))
                    return release_resources(INNER_ERROR, stack, q ,o_stack, data);

                if(!s_push(stack, new_symbol))
                    return release_resources(INNER_ERROR, stack, q, o_stack, data);

                if(new_symbol == P_ID){
                    if(!is_variable_defined(tree,data->token->data.string))
                        return release_resources(SEMANTIC_ERROR,stack, q, o_stack, data);
                }
//
//              if( new_symbol == P_ID || new_symbol == P_INT_NUM || new_symbol == P_FLOAT_NUM || new_symbol == P_STRING )


                if(tokenLookAheadFlag){
                    if(new_symbol == P_ID || new_symbol == P_STRING) {
                        free(data->token->data.string);
                    }
                    data->token = &aheadToken;
                    error = generate_postfix(data->token, o_stack, q);
                    if(error != 0)
                        return release_resources(error, stack, q, o_stack, data);
                    tokenLookAheadFlag = false;
                }
                else {
                    if(new_symbol == P_ID || new_symbol == P_STRING) {
                        free(data->token->data.string);
                    }
                    *data->token = nextToken();
                    error = generate_postfix(data->token, o_stack, q);
                    if(error != 0)
                        return release_resources(error, stack, q, o_stack, data);
                }

                new_symbol = token_to_symbol(*data->token);

                break;
            case X:
                if(DEBUG_EXPRESSION_ANALYSIS) printf("EXPRESSIONS: %s\n","I'm in X!");
                if (is_token_end_symbol(data->token) && *stack_top_terminal_symbol == P_DOLLAR) {
                    if(DEBUG_EXPRESSION_ANALYSIS) printf("EXPRESSIONS: %s\n","ENOUGH!, very nice");
                    enough = true;
                }
                else return release_resources(SYNTAX_ERROR, stack, q, o_stack, data);
                break;
            case E:
                if(DEBUG_EXPRESSION_ANALYSIS) printf("EXPRESSIONS: %s\n","I'm in E!");
                if(!s_push(stack,new_symbol))
                    return release_resources(INNER_ERROR, stack,  q, o_stack, data);
                if(new_symbol == P_ID || new_symbol == P_STRING) {
                    free(data->token->data.string);
                }
                *data->token = nextToken();
                new_symbol = token_to_symbol(*data->token);
                error = generate_postfix(data->token, o_stack, q);
                if(error != 0)
                    return release_resources(error, stack,  q, o_stack, data);

                break;
            default: break;
        }

    }
    print_queue(*q);
    error = queue_evaluation(q);

    return release_resources(error, stack, q , o_stack, data);

};

//int main(){
//    for(int i =0; i<3; i++){
//        printf("%d",nextToken().type);
//    }
//
//
//
//}

