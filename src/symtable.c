/*
 * Project: Implementace překladače imperativního jazyka IFJ18
 *
 * File: symtable.c
 *
 * xglosk00   Kristian Glos
 * xvagal00   Dominik Vagala
 * xvinsj00   Jakub Vins
 * xabike00   Zhamilya Abikenova
 *
 */
#include <stddef.h>
#include <stdlib.h>
#include <memory.h>
#include <stdio.h>
#include "string.h"
#include "symtable.h"

#include "errors.h"


//
// Created by Ancient on 22.11.2018.
//

Data_type get_type_from_token(tToken *token){
    switch (token->type){
        case INT:
            return type_integer;
        case FLOAT:
            return type_float;
        case STRING:
            return type_string;
        default:
            return type_undefined;
    }
}

//used to decide position of next node
int keyCmp(char *key1, char *key2, int letter_position ) {
    if (!strcmp(key1, key2)) {
        return TEQUAL; //equal
    } else if (key1[letter_position] == key2[letter_position]) { //if the letter is same, it goes to the next one
        keyCmp(key1, key2, ++letter_position);
    } else if (key1[letter_position] > key2[letter_position]) {//key2 is less
        return TLESS; //less
    }
    else return TMORE; // more

}

bool Binsert(Bnode *rootPtr, char *id, bool func_bool){

    if(!*rootPtr){
        Bnode new = malloc(sizeof(struct Node));
//        fprintf(stderr, "Malloc Bnode: %p\n", new);
        if(new) {
            // Copy so it is independant from token.data
            new->key = malloc(sizeof(char)*(strlen(id)+1));
            strcpy(new->key, id);
//            fprintf(stderr, "Malloc Bonode->key: %p, %s\n", new->key, id);
            new->data.function = func_bool;
            new->Lptr = NULL;
            new->Rptr = NULL;
            new->data.list = malloc(sizeof(List));
            new->data.list->element_count =0;
            listInit(new->data.list);
            *rootPtr = new;
            return true;
        }
    }
        //key comparison
    else {
        if((keyCmp((*rootPtr)->key, id, 0))==TEQUAL){ //nothing happens, if they are equal
            return true;
        }
        else if(keyCmp((*rootPtr)->key, id, 0)==TLESS){ // if added one is less we go to left
            Binsert(&(*rootPtr)->Lptr, id, func_bool);
        }
        else if((keyCmp((*rootPtr)->key, id, 0)==TMORE)){ // to right if it's more
            Binsert(&(*rootPtr)->Rptr, id, func_bool);
        }
        else
            return false;
    }

}

Bnode Bsearch(Bnode rootPtr, char *key ) {

    if (rootPtr) {
        if (keyCmp(rootPtr->key, key, 0) == TEQUAL) {
            return rootPtr;
        } else if (keyCmp(rootPtr->key, key, 0) == TLESS)
            Bsearch(rootPtr->Lptr, key);
        else Bsearch(rootPtr->Rptr, key);
    }
    else return NULL; // if it doesn't find anything
}
//updates bool "function: in data
void update_function(Bnode *rootPtr,char *key, bool function_bool){
    Bsearch(*rootPtr,key)->data.function = function_bool;
}

/**Same as local_symtable_init, but this add system functions to global_symtable*/
void global_symtable_init(Bnode *rootPtr){
    *rootPtr = NULL;

    // Populate symtable with system functions
    add_func_to_symtable(rootPtr, "inputs");
    Bnode pom = Bsearch(*rootPtr, "inputs");
    pom->data.system_function = true;

    add_func_to_symtable(rootPtr, "inputi");
    pom = Bsearch(*rootPtr, "inputi");
    pom->data.system_function = true;

    add_func_to_symtable(rootPtr, "inputf");
    pom = Bsearch(*rootPtr, "inputf");
    pom->data.system_function = true;

    add_func_to_symtable(rootPtr, "print");
    pom = Bsearch(*rootPtr, "print");
    pom->data.system_function = true;
    pom->data.list->element_count = -1;         // This means that number of print() parameters can vary

    add_func_to_symtable(rootPtr, "length");
    pom = Bsearch(*rootPtr, "length");
    pom->data.system_function = true;
    add_variable_to_func_params(rootPtr, "length", "s");

    add_func_to_symtable(rootPtr, "substr");
    pom = Bsearch(*rootPtr, "substr");
    pom->data.system_function = true;
    add_variable_to_func_params(rootPtr, "substr", "s");
    add_variable_to_func_params(rootPtr, "substr", "i");
    add_variable_to_func_params(rootPtr, "substr", "n");

    add_func_to_symtable(rootPtr, "ord");
    pom = Bsearch(*rootPtr, "ord");
    pom->data.system_function = true;
    add_variable_to_func_params(rootPtr, "ord", "s");
    add_variable_to_func_params(rootPtr, "ord", "i");

    add_func_to_symtable(rootPtr, "chr");
    pom = Bsearch(*rootPtr, "chr");
    pom->data.system_function = true;
    add_variable_to_func_params(rootPtr, "chr", "i");

}

void local_symtable_init(Bnode *rootPtr){
    *rootPtr = NULL;
}

bool is_variable_defined(Bnode *actual_symtable, char *var_name){
    Bnode pom = Bsearch(*actual_symtable,var_name);
    if(pom) {
            return true;
    }
    return false;
}

bool is_func_defined(Bnode *global_symtable, char *func_name){
    Bnode pom = Bsearch(*global_symtable, func_name);
    if(pom) {
        if (pom->data.function)
            return true;
    }
    return false;
}

bool is_system_function(Bnode *global_symtable, char *func_name){
    Bnode pom = Bsearch(*global_symtable, func_name);
    if(pom) {
        if (pom->data.system_function)
            return true;
    }
    return false;
}

bool is_variable_already_in_func_params(Bnode *global_symtable, char *func_name, char *var_name){
    Bnode pom = Bsearch(*global_symtable, func_name);
    if(pom) {
        if (is_element(pom->data.list,var_name))
            return true;
    }
    return false;
}

bool has_func_same_name_as_global_variable(Bnode *global_symtable, char *func_name){
    if(Bsearch(*global_symtable,func_name) != NULL)
        return true;
    return false;
}
bool has_variable_same_name_as_func(Bnode *global_symtable, char *var_name){
    Bnode pom = Bsearch(*global_symtable,var_name);
    if(pom != NULL && pom->data.function )
        return true;
    return false;
}

int get_num_of_defined_func_params(Bnode *global_symtable, char *func_name){
    Bnode pom = Bsearch(*global_symtable,func_name);
    if(pom){
        return pom->data.list->element_count;
    }
    return 0;
}

bool is_id_variable(Bnode *actual_symtable, char *var_name){
    Bnode pom = Bsearch(*actual_symtable, var_name);
    if(pom){
        if(pom->data.function == false)
            return true;

    }
    return false;
}

//---- These return false only when malloc fail creating new tree node,
bool add_variable_to_symtable(Bnode *actual_symtable, char *var_name){
    if (Binsert(actual_symtable, var_name, false))
        return true;
    return false;
}
bool add_func_to_symtable(Bnode *global_symtable, char *func_name){
    if (Binsert(global_symtable, func_name, true))
        return true;
    return false;
}

bool add_variable_to_func_params(Bnode *global_symtable, char *func_name, char *var_name){
    Bnode pom = Bsearch(*global_symtable,func_name);
    if(pom){
        if(pom->data.function){
            insert_element(pom->data.list, var_name, type_undefined);
            pom->data.list->element_count++;
            return true;
        }
    }
    return false;
}

void free_params(Bnode *global_symtable){

    if(*global_symtable){
        free_params(&(*global_symtable)->Rptr);
        free_params(&(*global_symtable)->Lptr);
        if((*global_symtable)->data.function == true){
            if((*(*global_symtable)->data.list).First != NULL){         // just functions that have parameters
//                fprintf(stderr, "Free: Param in Bnode: %p\n", (*(*global_symtable)->data.list).First);
                list_disposal((*global_symtable)->data.list);
            }
        }
    }
}

bool add_variables_from_func_params(Bnode *global_symtable, Bnode *actual_symtable, char *func_name){
    Bnode pom = Bsearch(*global_symtable,func_name);
    if (pom){
        for(int i = 0; i < pom->data.list->element_count; i++){
            if(!add_variable_to_symtable(actual_symtable, get_nth_element(pom->data.list,i)->id)) {
//                printf("%d",i);
                return false;
            }
        }
        return true;
    }
    return false;
}

char *get_name_of_defined_param_at_position(Bnode *global_symtable, char *func_name, int n){

    if(n >= get_num_of_defined_func_params(global_symtable, func_name))
        return NULL;

    Bnode pom = Bsearch(*global_symtable,func_name);
    if (pom){
        return get_nth_element(pom->data.list,n)->id;
    }
    return NULL;
}

void free_symtable(Bnode *symtable){

    if(*symtable){
        free_symtable(&(*symtable)->Rptr);
        free_symtable(&(*symtable)->Lptr);
//        fprintf(stderr, "Free: Bonode: %p\n", *symtable);
        free(*symtable);
        (*symtable) = NULL;
    }
}

void free_local_symtable(Bnode *symtable){
    free_symtable(symtable);
}

void free_global_symtable(Bnode *symtable){
    free_params(symtable);
    free_symtable(symtable);
}

//int main(){
//    Bnode tree = symtable_init();
//    Bnode local_tree = symtable_init();
//    add_variable_to_symtable(&tree,"ondo");
//    add_func_to_symtable(&tree, "orechi");
//    add_variable_to_func_params(&tree,"orechi","a");
//    add_variable_to_func_params(&tree,"orechi","b");
//    add_variable_to_func_params(&tree,"orechi","c");
//    if(add_variables_from_func_params(&tree,&local_tree,"orechi"))
//        printf("%s","juch");
//
//    if(is_variable_defined(&local_tree,"b"))
//        printf("%s","je");
//    if(is_variable_defined(&local_tree,"c"))
//        printf("%s","je");
//    printf("%d",Bsearch(tree,"orechi")->data.list->element_count);
//    free_symtable(&tree);
//}