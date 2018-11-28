//
// Created by Ancient on 25. 11. 2018.
//

#include "sym_stack.h"
#include "stdlib.h"


S_stack s_init(){
    S_stack *stack = malloc(sizeof(S_stack));
    stack->top = NULL;
    return *stack;
};

bool s_push(S_stack *stack, Prec_table_symbols_enum symbol, Data_type d_type ){

    S_item *item = (S_item*)malloc(sizeof(S_item));
    if(!item) return false;
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
    }
    return NULL;
}

int get_count_after_stop(S_stack *stack, bool *stop){
    int count = 0;
    S_item *item = stack->top;
    *stop = false;
    while(item){
        if(item->symbol != P_STOP) {
            count++;
            item = item->next_item;
        }
        else {
            *stop = true;
            break;
        }
    }
    return count;
}
void stack_n_pop(S_stack *stack, int n){
    for(int i = 0; i< n; i++ ){
        s_pop(stack);
    }
}

bool insert_after_top_terminal(S_stack *stack, Prec_table_symbols_enum symbol, Data_type d_type){
    if (stack) {
        S_item *pom = NULL;
        for (S_item *item = stack->top; item != NULL; item = item->next_item) {
            if (item->symbol < P_STOP) {
                S_item *new = (S_item *) malloc(sizeof(S_item));
                new->symbol = symbol;
                new->data_type = d_type;
                if (pom == NULL) {
                    stack->top = new;
                    new->next_item = item;
                } else {
                    item->next_item = pom->next_item;
                    pom->next_item = item;

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
}

//int main(){
//    S_stack stack = s_init();
//    s_push(&stack, P_DOLLAR, type_undefined);
//    s_push(&stack, P_NON_TERM, type_undefined);
//    printf("%d", get_top_terminal(&stack)->symbol);
//    insert_after_top_terminal(&stack, P_ID, type_undefined);
//
//}