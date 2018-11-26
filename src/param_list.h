//
// Created by Ancient on 25. 11. 2018.
//
#include <stdbool.h>
#ifndef IFJ_PROJECT_PARAM_LIST_H
#define IFJ_PROJECT_PARAM_LIST_H
typedef enum {
    type_integer,
    type_float,
    type_string,
    type_undefined,
    type_boolean
}Data_type;

typedef struct symbol_element{
    struct symbol_element *next;
    Data_type type ;
    char *id;
}L_element;


typedef struct {
    L_element *First;
    L_element *Last;
    int element_count; //count of elements in list
}List;

bool is_element(List *list, char *var_name);
int insert_element(List *list, char *id, Data_type type);
void listInit(List *list);
Data_type get_type_of_nth_element(List *list, int n);
void list_disposal(List *list);

#endif //IFJ_PROJECT_PARAM_LIST_H
