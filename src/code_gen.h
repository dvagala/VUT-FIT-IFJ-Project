//
// Created by Dominik Vagala on 28. 11. 2018.
//

#ifndef IFJ_PROJECT_CODE_GEN_H
#define IFJ_PROJECT_CODE_GEN_H

#include <stdio.h>


typedef struct Sstring{

    char *text;

    struct Sstring *next;
    struct Sstring *prev;
    struct Sstring *start;
    struct Sstring *end;

}*Tstring;

Tstring code_list;

void code_list_init();
void print_code();
char* append_text_to_last_string_in_code_list(char *string);
void add_allocated_string_to_code(char *string);
void add_const_string_to_code(char *string);
void free_code_list();

#endif //IFJ_PROJECT_CODE_GEN_H


