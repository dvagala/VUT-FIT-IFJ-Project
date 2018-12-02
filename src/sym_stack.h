/*
 * Project: Implementace překladače imperativního jazyka IFJ18
 *
 * File: sym_stack.h
 *
 * xglosk00   Kristian Glos
 * xvagal00   Dominik Vagala
 * xvinsj00   Jakub Vins
 * xabike00   Zhamilya Abikenova
 *
 */

#ifndef IFJ_PROJECT_SYM_STACK_H
#define IFJ_PROJECT_SYM_STACK_H


#include "symtable.h"

typedef struct stack_item{
    Prec_table_symbols_enum symbol;
    struct stack_item *next_item;
}S_item;

typedef struct
{
    S_item *top;
}S_stack;

void s_init(S_stack *stack);

bool s_push(S_stack *stack, Prec_table_symbols_enum symbol);

bool s_pop(S_stack *stack);

S_item *get_top_terminal(S_stack *stack);

int get_count_after_stop(S_stack stack);

void stack_n_pop(S_stack *stack, int n);

bool insert_after_top_terminal(S_stack *stack, Prec_table_symbols_enum symbol);

void s_free (S_stack *stack);

#endif //IFJ_PROJECT_SYM_STACK_H
