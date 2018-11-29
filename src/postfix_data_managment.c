#include <stddef.h>
#include <stdlib.h>
#include "postfix_data_managment.h"
#include "sym_stack.h"

//
// Created by vinso on 28.11.2018.
//
void operator_stack_init(Operator_stack *stack){
    stack->top = NULL;
};

bool operator_stack_push(Operator_stack *stack,Prec_table_symbols_enum operator){
    Operator_item *item = (Operator_item*)malloc(sizeof(Operator_item));
    if(!item) return false;

    item->next = stack->top;
    item->operator = operator;
    printf("%s","pushing operator to stack:");
    printf("%d\n",item->operator);
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

bool is_stack_top_not_null(Operator_stack *stack){
    if(stack->top != NULL)
        return true;
    else return false;
}

bool pop_to_output_queue(Operator_stack *stack, Output_queue *q){
    if(!(stack && q) )
        return false;
    Prec_table_symbols_enum *operator = &stack->top->operator;
    printf("%s","popin operator from stack:");
    printf("%d\n",*operator);
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

Output_queue queue_inint(Output_queue *q){
    q->first = NULL;
    q->last = NULL;
    return *q;
}

bool queue_insert(Output_queue *q, bool is_operator, int int_value, double float_value, char *string, Prec_table_symbols_enum operator){
    if (q){
        P_item *new =malloc(sizeof(P_item));
        if (!new) return false;
        new->value_int = int_value;
        new->value_double = float_value;
        new->string = string;
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

void update_last_is_operator(Output_queue *q){
    q->last->is_operator = true;
}


bool determine_type_and_insert(Output_queue *q, tToken *token){
    if(token->type == IDENTIFICATOR){
        if(queue_insert(q, false, 0,0, token->data.string, P_ID)) {
            update_last_is_variable(q);
            return true;
        }
        else
            return false;
    }

    switch (get_type_from_token(token)){
        case type_integer:
            if(queue_insert(q,false, token->data.value_int, 0, NULL, P_INT_NUM)) {
                return true;
            }
            else return false;
        case type_float:
            if(queue_insert(q, false , 0 ,token->data.value_double , NULL, P_FLOAT_NUM))
                return true;
            else return false;
        case type_string:
            if(queue_insert(q, false, 0, 0, token->data.string, P_STRING))
                return true;
            else return false;

        default: break;

    }

    return false;
}

bool insert_operator(Output_queue *q, Prec_table_symbols_enum *operator){
    if(q){
        if(queue_insert(q,true, 0, 0, NULL, *operator)) {

            return true;
        }
    }
    return false;
}

void print_queue(Output_queue q){
    P_item *item = q.first;
    printf("%s","postfix: ");
    while(item){
        if( item->operator == P_INT_NUM ) {
            printf("%d", item->value_int);
        }
        else if(item->operator == P_FLOAT_NUM)
            printf("%f", item->value_double);
        else  if((item->operator == P_STRING) || (item->is_variable)  )
            printf("%s", item->string);
        else if(item->is_operator){
            printf("%d",item->operator);
        }
        printf("%s"," ");
        item = item->next_item;
    }
    printf("%s\n","");
}

void queue_dispose(Output_queue *q) {
    while (q->first != NULL) {
        P_item *item = q->first;//pomocny element, potrebny na zapamatanie si prvku v zozname
        q->first = q->first->next_item;
        free(item);
    }
}

//int main(){
//    Operator_stack o_stack = operator_stack_init();
//    Output_queue q = queue_inint();
//    queue_insert(&q, false, type_undefined , NULL, NULL, "a", NULL);
//    update_last_is_variable(&q);
//    operator_stack_push(&o_stack,P_MINUS);
//    printf("%d\n",o_stack.top->operator);
//    pop_to_output_queue(&o_stack,&q);
//    //insert_operator(&q,P_PLUS);
//    //printf("%d",*q.last->operator);
//    queue_insert(&q, false, type_undefined , NULL, NULL, "b", NULL);
//    update_last_is_variable(&q);
//
//    queue_insert(&q, false, type_undefined , NULL, NULL, "c", NULL);
////    queue_insert(&q, false, type_undefined , NULL, NULL, "c", NULL);
//
//    update_last_is_variable(&q);
//
//
//    print_queue(q);
//}

