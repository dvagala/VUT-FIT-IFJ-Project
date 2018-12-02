//
// Created by Dominik Vagala on 28. 11. 2018.
//

#ifndef IFJ_PROJECT_CODE_GEN_H
#define IFJ_PROJECT_CODE_GEN_H

#include <stdio.h>
#include <stdbool.h>
#include "postfix_data_managment.h"


typedef struct Sstring{

    char *text;
    bool is_start_of_new_line;
    bool before_me_is_good_place_for_defvar;         // This will be true if string is "while"

    struct Sstring *next;
    struct Sstring *prev;
    struct Sstring *start;
    struct Sstring *end;

}*Tstring;

Tstring active_code_list;           // This is either main_code_list or functions_code_list
Tstring main_code_list;
Tstring functions_code_list;

// This is used to create unique names for generic labels, after every generic label append this number
int generic_label_count;


void code_list_init();
void print_code();
void print_code_backwards();
char* append_text_to_specific_string(Tstring specific_string, char *text);
Tstring find_nearest_good_place_for_defvar();
void add_string_after_specific_string(Tstring specific_string, char *text);
void free_code_lists();

bool declare_defvar_restype();
bool insert_defvar_res();
bool insert_simple_instruction(char *instruction);
bool insert_instruction(char *instruction, P_item *o1, P_item *o2, P_stack *post_stack);
bool top_value_gen_and_add(P_stack *post_stack);


char* convert_string_to_correct_IFJcode18_format(char *input);

void generate_system_functions();

#endif //IFJ_PROJECT_CODE_GEN_H


