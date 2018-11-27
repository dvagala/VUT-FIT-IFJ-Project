//
// Created by Ancient on 25. 11. 2018.
//

#include <stddef.h>
#include <stdlib.h>
#include "param_list.h"
#include "errors.h"
#include "string.h"

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

bool is_element(List *list, char *var_name){
    L_element *pom = list->First;
    while(pom){
        if(strcmp(var_name, pom->id) == 0)
            return true;
        pom = pom->next;
    }
    return false;
}

//first element is 0
L_element *get_nth_element(List *list, int n){
    L_element *pom=list->First;
    if (n >= list->element_count)
        return NULL;
    for(int i = 0;i<n;i++){
        pom = pom->next;
    }
    return pom;
}

void list_disposal(List *list){
    while (list->First!=NULL){
        L_element *e = list->First;//pomocny element, potrebny na zapamatanie si prvku v zozname
        list->First = list->First->next;
        free(e);
    }
}