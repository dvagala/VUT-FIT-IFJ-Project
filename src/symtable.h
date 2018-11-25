//
// Created by Ancient on 22. 11. 2018.
//

#ifndef IFJ_PROJECT_SYMTABLE_H
#define IFJ_PROJECT_SYMTABLE_H
#include "param_list.h"
#include <stdbool.h>

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
#endif //IFJ_PROJECT_SYMTABLE_H
