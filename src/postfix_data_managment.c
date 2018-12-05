/*
 * Project: Implementace překladače imperativního jazyka IFJ18
 *
 * File: postfix_data_managment.c
 *
 * xglosk00   Kristian Glos
 * xvagal00   Dominik Vagala
 * xvinsj00   Jakub Vins
 * xabike00   Zhamilya Abikenova
 *
 */

#include <stddef.h>
#include <stdlib.h>
#include "postfix_data_managment.h"
#include "sym_stack.h"


#define DEBUG_POSTFIX 0         // Set to '1', if you want to print debug stuff
\
/**Operator stack
 * Basic stack structure that is designed to store expression operators.*/
void operator_stack_init(Operator_stack *stack){
    stack->top = NULL;
};


bool operator_stack_push(Operator_stack *stack,Prec_table_symbols_enum operator){
    Operator_item *item = (Operator_item*)malloc(sizeof(Operator_item));
    if(!item) return false;

    item->next = stack->top;
    item->operator = operator;
    if(DEBUG_POSTFIX) printf("POSTFIX: %s","pushing operator to stack:");
    if(DEBUG_POSTFIX) printf("POSTFIX: %d\n",item->operator);
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
    Prec_table_symbols_enum *operator = &stack->top->operator;
    if(DEBUG_POSTFIX) printf("POSTFIX: %s","popin operator from stack:");
    if(DEBUG_POSTFIX) printf("POSTFIX: %d\n",*operator);
    if(insert_operator(q, *operator))
        if(operator_pop(stack))
            return true;
    return false;
}

void operator_stack_free (Operator_stack *stack){
    while(stack->top){
        operator_pop(stack);
    }
    operator_stack_init(stack);

}
//--------------------------------------------------
/**Postfix stack
 * Another stack structure designed to store operands (in form of a P_item) from postfix output queue
 * */

void p_stack_init(P_stack *stack) {
    if (stack) {
        stack->top = NULL;
    }
}

bool p_stack_push(P_stack *stack, bool is_variable, int int_value, double float_value, char *string, Prec_table_symbols_enum type){
    if(stack){
        P_item *new = (P_item*)malloc(sizeof(struct postfix_stack_item));
        if(DEBUG_POSTFIX) printf("POSTFIX: %s%d\n","I am pushin onto post_stack: ", int_value);
        if(!new)
            return false;
        new->value_int = int_value;
        new->value_double = float_value;
        if(string != NULL) {
            new->string = malloc(sizeof(char)*(strlen(string)+1));
            strcpy(new->string, string);
        }
        else new->string = NULL;
        new->operator = type;
        new->next_item = stack->top;
        new->is_variable = is_variable;
        new->is_operator = false;
        new->res= false;
        stack->top = new;
        return true;
    }
    return false;

}

bool determine_type_and_push(P_stack *stack, P_item *item){

    if(item->operator == P_ID){
        if(p_stack_push(stack, true, 0,0, item->string, P_ID)) {
            return true;
        }
        else
            return false;
    }

    switch (item->operator){
        case P_INT_NUM:
            if(p_stack_push(stack,false, item->value_int, 0, NULL, P_INT_NUM)) {
                return true;
            }
            else return false;
        case P_FLOAT_NUM:
            if(p_stack_push(stack, false , 0 ,item->value_double , NULL, P_FLOAT_NUM))
                return true;
            else return false;
        case P_STRING:
            if(p_stack_push(stack, false, 0, 0, item->string, P_STRING))
                return true;
            else return false;

        default: break;

    }

    return false;
}

void p_stack_pop(P_stack *stack){
    if(stack)
        if(stack->top!=NULL){
            P_item *pom = stack->top;
            stack->top = stack->top->next_item;
            if(pom->string!=NULL) {
                free(pom->string);
            }
            free(pom);
        }
}

void p_stack_free(P_stack *stack){
    if(stack){
        while(stack->top){
            p_stack_pop(stack);
        }
    }
    p_stack_init(stack);

}




//---------------------------------------------------
/**Output queue
 * Queue that is more of a list as a result of my not exactly right calculations. Stores postfix, and enables transfering
 * values from it to postfix stack.
 * */
void queue_inint(Output_queue *q){
    q->first = NULL;
    q->last = NULL;
}

bool queue_insert(Output_queue *q, bool is_operator, int int_value, double float_value, char *string, Prec_table_symbols_enum operator){
    if (q){
        P_item *new =malloc(sizeof(P_item));
        if (!new) return false;
        new->value_int = int_value;
        new->value_double = float_value;
        if(string != NULL) {
            new->string = malloc(sizeof(char)*(strlen(string)+1));
            strcpy(new->string, string);
        }
        else new->string = NULL;
        new->is_operator = is_operator;
        new->is_variable = false;
        new->operator = operator;
        new->next_item = NULL;
        new->res = false;
        if(q->first == NULL){
            q->first = new;
        }
        else {
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

bool insert_operator(Output_queue *q, Prec_table_symbols_enum operator){
    if(q){
        if(queue_insert(q,true, 0, 0, NULL, operator)) {

            return true;
        }
    }
    return false;
}

void print_queue(Output_queue q){
    P_item *item = q.first;
    if(DEBUG_POSTFIX) printf("POSTFIX: %s\n","postfix: ");
    while(item){
        if( item->operator == P_INT_NUM ) {
            if(DEBUG_POSTFIX) printf("%d", item->value_int);
        }
        else if(item->operator == P_FLOAT_NUM) {
            if (DEBUG_POSTFIX) printf("%f", item->value_double);
        }
        else if((item->operator == P_STRING) || (item->is_variable)  ) {
            if (DEBUG_POSTFIX) printf("%s", item->string);
        }
        else if(item->is_operator){
            if(DEBUG_POSTFIX) printf("%d",item->operator);
        }
        if(DEBUG_POSTFIX) printf("%s"," ");
        item = item->next_item;
    }
    if(DEBUG_POSTFIX) printf("%s\n","");
}
void delete_first(Output_queue *q){
    if(q->first != NULL){
            P_item *pom = q->first;
           if(q->first==q->last){
                    queue_inint(q);
            }
           else q->first = pom->next_item;
            free(pom->string);
            free(pom);

    }
}

void queue_dispose(Output_queue *q) {
    if (!q) return ;
    P_item *pom;
    while ((pom= q->first)) {
        delete_first(q);
    }
    queue_inint(q);

}

//------------------------------------------------------------

bool first_from_queue_to_stack(Output_queue *q, P_stack *stack){

    if(q && stack) {
        if (!determine_type_and_push(stack, q->first))
            return false;
        delete_first(q);
        return true;
    }
    return false;
}


