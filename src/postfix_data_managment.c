#include <stddef.h>
#include <stdlib.h>
#include "postfix_data_managment.h"
#include "sym_stack.h"

//
// Created by vinso on 28.11.2018.
//
Operator_stack operator_stack_init(){
    Operator_stack *stack = malloc(sizeof(Operator_stack));
    stack->top = NULL;
    return *stack;
};

bool operator_stack_push(Operator_stack *stack,Prec_table_symbols_enum operator){
    Operator_item *item = (Operator_item*)malloc(sizeof(Operator_item));
    if(!item) return false;
    item->next = stack->top;
    item->operator = operator;
    stack->top = item;
    return true;
}

bool operator_pop(Operator_stack *stack){
    if(stack->top) {
        Operator_item *pom = stack->top;
        stack->top = stack->top->next;
        free(pom);
        return true;
    }
    return false;
}


bool pop_to_output_queue(Operator_stack *stack, Output_queue *q){
    if(!(stack && q) )
        return false;
    Prec_table_symbols_enum operator = stack->top->operator;
    if(insert_operator(q, operator))
        if(operator_pop(stack))
            return true;
    return false;
}

void operator_stack_free (Operator_stack *stack){
    while(stack->top){
        operator_pop(stack);
    }
}

//---------------------------------------------------

Output_queue queue_inint(){
    Output_queue *q = (Output_queue*)malloc(sizeof(Output_queue));
    q->first = NULL;
    q->last = NULL;
    return *q;
}

bool queue_insert(Output_queue *q, bool is_operator, Data_type type, int *int_value, double *float_value, char *string, Prec_table_symbols_enum *operator){
    if (q){
        P_item *new =(P_item*)malloc(sizeof(P_item));
        if (!new) return false;
        new->value_int = int_value;
        new->value_double = float_value;
        new->string = string;
        new->type = &type;
        new->is_operator = is_operator;
        new->is_variable = false;
        new->operator = operator;
        new->next_item = NULL;
        if(q->first == NULL){
            q->first = new;
        }
        else if(q->first != NULL){
            q->last->next_item = new;
        }
        q->last = new;
        return true;

    }
    return false;

}

void update_last_is_variable(Output_queue *q){
    q->last->is_variable = true;
}

bool determine_type_and_insert(Output_queue *q, tToken *token){
    if(token->type == IDENTIFICATOR){
        if(queue_insert(q, false, type_string, NULL, NULL, token->data.string, NULL))
            return true;
        else return false;
    }

    switch (get_type_from_token(token)){
        case type_integer:
            if(queue_insert(q,false,type_integer, &token->data.value_int, NULL, NULL, NULL))
                return true;
            else return false;
        case type_float:
            if(queue_insert(q, false ,type_float ,NULL ,&token->data.value_double , NULL, NULL))
                return true;
            else return false;
        case type_string:
            if(queue_insert(q, false, type_string, NULL, NULL, token->data.string, NULL))
                return true;
            else return false;

        default: break;

    }

    return false;
}

bool insert_operator(Output_queue *q, Prec_table_symbols_enum operator){
    if(q){
        if(queue_insert(q,true,type_undefined , NULL, NULL, NULL, &operator))
            return true;
    }
    return false;
}

void queue_dispose(Output_queue *q) {
    while (q->first != NULL) {
        P_item *item = q->first;//pomocny element, potrebny na zapamatanie si prvku v zozname
        q->first = q->first->next_item;
        free(item);
    }
}

