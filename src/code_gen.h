//
// Created by Dominik Vagala on 28. 11. 2018.
//

#ifndef IFJ_PROJECT_CODE_GEN_H
#define IFJ_PROJECT_CODE_GEN_H

#include <stdio.h>
#include <stdbool.h>


typedef struct Sstring{

    char *text;
    bool is_start_of_new_line;
    bool before_me_is_good_place_for_defvar;         // This will be true if string is "while"

    struct Sstring *next;
    struct Sstring *prev;
    struct Sstring *start;
    struct Sstring *end;

}*Tstring;

Tstring code_list;

void code_list_init();
void print_code();
void print_code_backwards();
char* append_text_string_to_specific_string(Tstring specific_string, char *text);
char* append_allocated_string_to_specific_string(Tstring specific_string, char *text);
Tstring find_nearest_good_place_for_defvar();
void add_allocated_string_after_specific_string(Tstring specific_string, char *text);
void add_text_string_after_specific_string(Tstring specific_string, char *text);
void free_code_list();

#endif //IFJ_PROJECT_CODE_GEN_H


