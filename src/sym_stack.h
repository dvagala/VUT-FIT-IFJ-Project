//
// Created by Ancient on 25. 11. 2018.
//

#ifndef IFJ_PROJECT_SYM_STACK_H
#define IFJ_PROJECT_SYM_STACK_H

#include "expressions.h"
#include "symtable.h"
#include <stdbool.h>
typedef struct stack_item{
    Data_type data_type;
    Prec_table_symbols_enum symbol;
    struct stack_item *next_item;
}S_item;

typedef struct
{
    S_item *top;
}S_stack;

void s_init(S_stack *stack);

bool s_push(S_stack *stack, Prec_table_symbols_enum symbol, Data_type d_type );

bool s_pop(S_stack *stack);

S_item *get_top_terminal(S_stack *stack);

void stack_n_pop(S_stack *stack, int n);

bool insert_after_top_terminal(S_stack *stack, Prec_table_symbols_enum symbol, Data_type d_type);

S_item *s_top (S_stack *stack);

void s_free (S_stack *stack);

#endif //IFJ_PROJECT_SYM_STACK_H