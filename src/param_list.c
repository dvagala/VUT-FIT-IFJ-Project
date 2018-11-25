//
// Created by Ancient on 25. 11. 2018.
//

#include <stddef.h>
#include <stdlib.h>
#include "param_list.h"
#include "errors.h"

void listInit(List *list){
    list->First = NULL;
    list->Last = NULL;
}

int insert_element(List *list, char *id, Data_type type){
    if(!list) return INNER_ERROR;
    L_element *new = malloc(sizeof(L_element));
    if(!new) return INNER_ERROR;
    new->type = type;
    new->next = NULL;
    new->id = id;

    if(list->First == NULL){
        list->First = new;
    }
    else{
        list->Last->next = new;
    }
    list->Last = new;
    return 1;
}
//first element is 0
Data_type get_type_of_nth_element(List *list, int n){
    L_element *pom=list->First;
    if (n >= list->element_count)
        return DIFF_ERROR;
    for(int i = 0;i<n;i++){
        pom = pom->next;
    }
    return pom->type;
}

void list_disposal(List *list){
    while (list->First!=NULL){
        L_element *e = list->First;//pomocny element, potrebny na zapamatanie si prvku v zozname
        list->First = list->First->next;
        free(e);
    }
}