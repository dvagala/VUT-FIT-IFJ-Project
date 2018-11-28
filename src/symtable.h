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
    bool function;              // function or variable
    bool system_function;       // system function or (user function, variable)
    List *list;
}Bdata;

typedef struct Node{
    char *key;
    Bdata data;
    struct Node *Lptr;
    struct Node *Rptr;
}*Bnode;

Data_type get_type_from_token(tToken *token);

bool Binsert(Bnode *rootPtr,char *id, bool func_bool);

void update_function(Bnode *rootPtr,char *key, bool function_bool);

void global_symtable_init(Bnode *rootPtr);

void local_symtable_init(Bnode *rootPtr);

bool is_id_variable(Bnode *actual_symtable, char *var_name);

bool is_variable_defined(Bnode *actual_symtable, char *var_name);

bool is_func_defined(Bnode *global_symtable, char *func_name);

bool is_variable_already_in_func_params(Bnode *global_symtable, char *func_name, char *var_name);

bool has_func_same_name_as_global_variable(Bnode *global_symtable, char *func_name);

bool has_variable_same_name_as_func(Bnode *global_symtable, char *var_name);

int get_num_of_defined_func_params(Bnode *global_symtable, char *func_name);

bool add_variable_to_symtable(Bnode *actual_symtable, char *var_name);

bool add_func_to_symtable(Bnode *global_symtable, char *func_name);

bool add_variable_to_func_params(Bnode *global_symtable, char *func_name, char *var_name);

bool add_variables_from_func_params(Bnode *global_symtable, Bnode *actual_symtable, char *func_name);

void free_symtable(Bnode *symtable);

#endif //IFJ_PROJECT_SYMTABLE_H
