//
// Created by Ancient on 22. 11. 2018.
//

#ifndef IFJ_PROJECT_SYMTABLE_H
#define IFJ_PROJECT_SYMTABLE_H
#include "param_list.h"

#include <stdbool.h>
#include "expressions.h"

#define TEQUAL 0
#define TLESS  1
#define TMORE 2



typedef struct {
    Data_type type;
    bool function;
    List *list;
    bool defined;
}Bdata;

typedef struct Node{
    char *key;
    Bdata data;
    struct Node *Lptr;
    struct Node *Rptr;
}*Bnode;

int Binsert(Bnode *rootPtr,tToken *token);
void update_function(Bnode *rootPtr,char *key, bool bulo);
void update_defined(Bnode *rootPtr,char *key, bool bulo);
void update_type(Bnode *rootPtr,char *key, Data_type type);
bool isDefined(Bnode rootPtr, char *key);
void add_param(Bnode *globalRoot, char *funcName, tToken *token);
void BDispose(Bnode *rootPtr);
#endif //IFJ_PROJECT_SYMTABLE_H



// TODO Functions for parser

bool is_id_variable(char *var_name);
bool is_variable_defined(Bnode *actual_symtable, char *var_name);
bool is_func_defined(Bnode *global_symtable, char *func_name);
bool is_variable_already_in_func_params(Bnode *global_symtable, char *func_name, char *var_name);
bool has_func_same_name_as_global_variable(Bnode *global_symtable, char *func_name);
bool has_variable_same_name_as_func(Bnode *global_symtable, char *var_name);

int get_num_of_defined_func_params(Bnode *global_symtable, char *func_name);

// These return false only when malloc fail creating new tree node,
bool add_variable_to_symtable(Bnode *actual_symtable);
bool add_func_to_symtable(Bnode *global_symtable);
bool add_variable_to_func_params(Bnode *global_symtable, char *func_name, char *var_name);


// Return pointer to new symtable, NULL when malloc fail or something
Bnode* create_symtable();

// Return false only when free fail or something (maybe not neccessary)
bool free_symtable(Bnode *symtable);
