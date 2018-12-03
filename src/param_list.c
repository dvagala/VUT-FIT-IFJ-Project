/*
 * Project: Implementace překladače imperativního jazyka IFJ18
 *
 * File: param_list.c
 *
 * xglosk00   Kristian Glos
 * xvagal00   Dominik Vagala
 * xvinsj00   Jakub Vins
 * xabike00   Zhamilya Abikenova
 *
 */

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include "param_list.h"
#include "errors.h"
#include "string.h"

#define DEBUG_FREE 0

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
    new->id = malloc(sizeof(char) * (strlen(id)+1));
    strcpy(new->id, id);
//    if(DEBUG_FREE) fprintf(stderr, "Malloc: Param: %p,\n", new);
//    if(DEBUG_FREE) fprintf(stderr, "Malloc: Param->id: %p, %s\n", new->id, new->id);

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
        if(DEBUG_FREE) fprintf(stderr, "Free: Param in Bnode: %p, %s\n", e, e->id);
        free(e->id);
        free(e);
    }
}
