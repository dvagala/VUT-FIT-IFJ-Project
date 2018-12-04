/*
 * Project: Implementace překladače imperativního jazyka IFJ18
 *
 * File: sym_stack.c
 *
 * xglosk00   Kristian Glos
 * xvagal00   Dominik Vagala
 * xvinsj00   Jakub Vins
 * xabike00   Zhamilya Abikenova
 *
 */

#include "sym_stack.h"
#include "stdlib.h"


void s_init(S_stack *stack){

    stack->top = NULL;
};

bool s_push(S_stack *stack, Prec_table_symbols_enum symbol ){

    S_item *item = (S_item*)malloc(sizeof(S_item));
    if(!item) return false;
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
    }
    return NULL;
}

int get_count_after_stop(S_stack stack){
    int count = 0;
    S_item *item = stack.top;

    while(item){
        if(item->symbol != P_STOP) {
            count++;
        }
        else {
            return count;
        }
        item = item->next_item;
    }

    return 0;
}
void stack_n_pop(S_stack *stack, int n){
    for(int i = 0; i< n; i++ ){
        s_pop(stack);
    }
}

bool insert_after_top_terminal(S_stack *stack, Prec_table_symbols_enum symbol){
    if (stack) {
        S_item *pom = NULL;
        for (S_item *item = stack->top; item != NULL; item = item->next_item) {
            if (item->symbol < P_STOP) {
                S_item *new = (S_item *) malloc(sizeof(S_item));
                if(!new)
                    return false;
                new->symbol = symbol;
                if (pom == NULL) {
                    new->next_item = stack->top;
                    stack->top = new;
                } else {
                    new->next_item = pom->next_item;
                    pom->next_item = new;

                }
                return true;
            }
            pom = item;
        }
    }
    return false;

}


void s_free (S_stack *stack){
    while(stack->top){
        s_pop(stack);
    }
    s_init(stack);
}

//int main(){
//    S_stack *stack =  malloc(sizeof(S_stack));
//    s_init(stack);
//    s_push(stack, P_DOLLAR);
//    s_push(stack, P_NON_TERM);
//    printf("%d",get_top_terminal(stack)->symbol);
//
//
//
//    printf("%d",get_count_after_stop(*stack));
////    printf("%d", get_top_terminal(stack)->symbol);
////    insert_after_top_terminal(stack, P_ID);
//
//}