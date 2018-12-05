/*
 * Project: Implementace překladače imperativního jazyka IFJ18
 *
 * File: code_gen.h
 *
 * xglosk00   Kristian Glos
 * xvagal00   Dominik Vagala
 * xvinsj00   Jakub Vins
 * xabike00   Zhamilya Abikenova
 *
 */

#ifndef IFJ_PROJECT_CODE_GEN_H
#define IFJ_PROJECT_CODE_GEN_H

#include <stdio.h>
#include <stdbool.h>
#include "postfix_data_managment.h"


typedef struct Sstring{

    char *text;
    bool is_start_of_new_line;
    bool before_me_is_good_place_for_defvar;      // This will be true if this is start of while loop, or if statement

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
char* append_text_to_specific_string(Tstring specific_string, char *text);
Tstring find_nearest_good_place_for_defvar();
void add_string_after_specific_string(Tstring specific_string, char *text);
void free_code_lists();

void gen_custom_jumpifeq(P_item *o1, Prec_table_symbols_enum operator);
void gen_unique_operation(Prec_table_symbols_enum operator, P_item *o1);
void exit_gen(int error_code);
void push_res();
void declare_defvar_restype(bool is_while);
void insert_defvar_res(bool is_while);
void insert_simple_instruction(char *instruction);
void insert_instruction(char *instruction, P_item *o1, P_item *o2, char *type);
void item_value_gen_and_add(P_item *item, bool append);
void insert_res_and_type( P_item *o1);
void gen_defvar_in_while(P_item *o1);
char* convert_string_to_correct_IFJcode18_format(char *input);

void generate_system_functions();

#endif //IFJ_PROJECT_CODE_GEN_H


