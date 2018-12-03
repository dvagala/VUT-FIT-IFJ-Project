/*
 * Project: Implementace překladače imperativního jazyka IFJ18
 *
 * File: postfix_data_managment.h
 *
 * xglosk00   Kristian Glos
 * xvagal00   Dominik Vagala
 * xvinsj00   Jakub Vins
 * xabike00   Zhamilya Abikenova
 *
 */


#ifndef IFJ_PROJECT_POSTFIX_STACK_H
#define IFJ_PROJECT_POSTFIX_STACK_H


#include "param_list.h"
#include "scanner.h"
#include "expressions.h"

typedef struct operator_stack_item{
    Prec_table_symbols_enum operator;
    struct operator_stack_item *next;
}Operator_item;

typedef struct
{
    Operator_item *top;
}Operator_stack;



typedef struct postfix_stack_item{
    int    value_int;
    double value_double;
    char  *string;
    Prec_table_symbols_enum operator;
    bool is_operator;
    bool is_variable;
    bool res;
    struct postfix_stack_item *next_item;
}P_item,*P_item_ptr;

typedef struct p_item_stack{
    P_item *top;
}P_stack, *P_stack_ptr;


typedef struct {
    P_item_ptr first;
    P_item_ptr last;
}Output_queue;

void operator_stack_init(Operator_stack *stack);
bool operator_stack_push(Operator_stack *stack,Prec_table_symbols_enum operator);
bool operator_pop(Operator_stack *stack);
bool pop_to_output_queue(Operator_stack *stack, Output_queue *q);
void operator_stack_free (Operator_stack *stack);
bool is_stack_top_not_null(Operator_stack *stack);

void p_stack_init(P_stack *stack);
bool p_stack_push(P_stack *stack, bool is_variable, int int_value, double float_value, char *string, Prec_table_symbols_enum type);
bool determine_type_and_push(P_stack *stack, P_item *item);
void p_stack_pop(P_stack *stack);
void p_stack_free(P_stack *stack);

void print_queue(Output_queue q);
void queue_inint(Output_queue *q);
bool queue_insert(Output_queue *q, bool is_operator, int int_value, double float_value, char *string, Prec_table_symbols_enum operator);
bool insert_operator(Output_queue *q, Prec_table_symbols_enum operator);
void queue_dispose(Output_queue *q);
bool determine_type_and_insert(Output_queue *q, tToken *token);
void delete_first(Output_queue *q);
bool first_from_queue_to_stack(Output_queue *q, P_stack *stack);
void update_res(P_item *item);
P_item* get_first_operand(Output_queue *q);
P_item* get_second_operand(Output_queue *q);
void delete_until_operator(Output_queue *q);
#endif //IFJ_PROJECT_POSTFIX_STACK_H
