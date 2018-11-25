//
// Created by Ancient on 25. 11. 2018.
//
#include <stdbool.h>
#include "sym_stack.h"
#include "stdlib.h"
#include "symtable.h"

void s_init(S_stack *stack){
    stack->top = NULL;
};

bool s_push(S_stack *stack, Prec_table_symbols_enum symbol, Data_type d_type ){

    S_item *item = (S_item*)malloc(sizeof(S_item));
    item->data_type = d_type;
    item->next_item = stack->top;
    item->symbol = symbol;
    stack->top = item;
    return true;
}

bool s_pop(S_stack *stack) {
    if(stack->top) {
        S_item *pom = stack->top;
        stack->top = stack->top->next_item;
        free(pom);
        return true;
    }
    return false;
}

S_item *get_top_terminal(S_stack *stack){
    for (S_item *pom = stack->top; pom != NULL; pom = pom->next_item){
        if(pom->symbol < P_STOP){
            return pom;
        }
        return NULL;
    }
}

void stack_n_pop(S_stack *stack, int n){
    for(int i = 0; i< n; i++ ){
        s_pop(stack);
    }
}

bool insert_after_top_terminal(S_stack *stack, Prec_table_symbols_enum symbol, Data_type d_type){

    S_item *pom = NULL;
    for(S_item *item = stack->top; item != NULL; item = item->next_item){
        if(item->symbol < P_STOP){
            S_item *new = (S_item*)malloc(sizeof(S_item));
            new->symbol = symbol;
            new->data_type = d_type;
            if (pom == NULL){
                stack->top = new;
                new->next_item = item;
            }
            else {
                item->next_item = pom->next_item ;
                pom->next_item = item;

            }
            return true;
        }
        pom = item;
    }
    return false;

}

S_item *s_top (S_stack *stack){
    return stack->top;
}

void s_free (S_stack *stack){
    while(stack->top){
        s_pop(stack);
    }
}
