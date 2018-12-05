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


/**Function returns data_type from token*/
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

/**Function that compares keys in tree structure and decides if key2 is less, more or equal */
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

/**Function for inserting Bnode into the tree structure */
bool Binsert(Bnode *rootPtr, char *id, bool func_bool){

    if(!*rootPtr){
        Bnode new = malloc(sizeof(struct Node));
        if(new) {
            // Copy so it is independant from token.data
            new->key = malloc(sizeof(char)*(strlen(id)+1));
            strcpy(new->key, id);
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

/**Function that enables searching for Bnode of a certain key and returns it if found. */
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
    add_func_to_symtable(rootPtr, "print");
    Bnode pom = Bsearch(*rootPtr, "print");
    pom->data.system_function = true;
    pom->data.list->element_count = -1;         // This means that number of print() parameters can vary

    add_func_to_symtable(rootPtr, "inputs");
    pom = Bsearch(*rootPtr, "inputs");
    pom->data.system_function = true;

    add_func_to_symtable(rootPtr, "inputi");
    pom = Bsearch(*rootPtr, "inputi");
    pom->data.system_function = true;

    add_func_to_symtable(rootPtr, "inputf");
    pom = Bsearch(*rootPtr, "inputf");
    pom->data.system_function = true;

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

/**Initializes tree */
void local_symtable_init(Bnode *rootPtr){
    *rootPtr = NULL;
}
/**Returns true if variable is already defined in tree */
bool is_variable_defined(Bnode *actual_symtable, char *var_name){
    Bnode pom = Bsearch(*actual_symtable,var_name);
    if(pom) {
            return true;
    }
    return false;
}

/**Returns true if function is already defined in tree*/
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

/**Returns true if variable is already in list of a function parameters */
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

/**These functions return false, when malloc fails*/
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

/**This function return id of a parameter at certain position */
char *get_name_of_defined_param_at_position(Bnode *global_symtable, char *func_name, int n){

    if(n >= get_num_of_defined_func_params(global_symtable, func_name))
        return NULL;

    Bnode pom = Bsearch(*global_symtable,func_name);
    if (pom){
        return get_nth_element(pom->data.list,n)->id;
    }
    return NULL;
}

/**Functions for tree disposal*/
void free_symtable_recursive(Bnode *symtable){

    if(*symtable){
        free_symtable_recursive(&(*symtable)->Rptr);
        free_symtable_recursive(&(*symtable)->Lptr);
        free((*symtable)->data.list);
        free((*symtable)->key);
        (*symtable)->key = NULL;
        free(*symtable);
        (*symtable) = NULL;
    }
}

void free_params(Bnode *global_symtable){

    if(*global_symtable){
        free_params(&(*global_symtable)->Rptr);
        free_params(&(*global_symtable)->Lptr);
        if((*global_symtable)->data.function == true){
            if((*(*global_symtable)->data.list).First != NULL){         // just functions that have parameters
                list_disposal((*global_symtable)->data.list);
            }
        }
    }
}

void free_symtable(Bnode *symtable){
    free_params(symtable);
    free_symtable_recursive(symtable);
}
