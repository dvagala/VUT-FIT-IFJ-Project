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
