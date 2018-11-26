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

void Binit(Bnode *rootPtr){
    *rootPtr = NULL;
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
        if(new) {
            new->key = id;
            new->data.function = func_bool;
            new->Lptr = NULL;
            new->Rptr = NULL;
            new->data.list = malloc(sizeof(List));
            new->data.list->element_count =0;
            listInit(new->data.list);
            *rootPtr = new;
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
    }
    return false;
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
//----

void free_symtable(Bnode *symtable){
    if(*symtable){
        free_symtable(&(*symtable)->Rptr);
        free_symtable(&(*symtable)->Lptr);
        free(*symtable);
        Binit(&(*symtable));
    }
}

//int main(){
//    Bnode *tree ;
//    Binit(tree);
//    if(is_variable_defined(tree,"orech"))
//        printf("%s\n", "je");
//    add_variable_to_symtable(tree, "orech");
//    if(is_variable_defined(tree,"orech"))
//        printf("%s\n", "je");
//    add_variable_to_symtable(tree, "orecho");
//    add_variable_to_symtable(tree, "oreche");
//    add_func_to_symtable(tree, "orechi");
//
//    add_variable_to_func_params(tree,"orechi", "ondrej");
//    if(is_variable_already_in_func_params(tree,"orechi","ondrej"))
//        printf("%s\n", "je");
//    printf("%d\n", get_num_of_defined_func_params(tree,"orechi"));
//    if(has_func_same_name_as_global_variable(tree,"orechl"))
//        printf("%s\n", "ma");
//    if(has_variable_same_name_as_func(tree,"orechl"))
//        printf("%s\n", "maa");
//    free_symtable(tree);
//    if(tree)
//        printf("%s\n", "existuje");
//}