//
// Created by vinso on 28.11.2018.
//

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
    struct postfix_stack_item *next_item;
}P_item;

typedef struct {
    P_item *first;
    P_item *last;
}Output_queue;

void operator_stack_init(Operator_stack *stack);
bool operator_stack_push(Operator_stack *stack,Prec_table_symbols_enum operator);
bool operator_pop(Operator_stack *stack);
bool pop_to_output_queue(Operator_stack *stack, Output_queue *q);
void operator_stack_free (Operator_stack *stack);
bool is_stack_top_not_null(Operator_stack *stack);

void print_queue(Output_queue q);
Output_queue queue_inint(Output_queue *q);
bool queue_insert(Output_queue *q, bool is_operator, int int_value, double float_value, char *string, Prec_table_symbols_enum operator);
bool insert_operator(Output_queue *q, Prec_table_symbols_enum *operator);
void queue_dispose(Output_queue *q);
bool determine_type_and_insert(Output_queue *q, tToken *token);
#endif //IFJ_PROJECT_POSTFIX_STACK_H
