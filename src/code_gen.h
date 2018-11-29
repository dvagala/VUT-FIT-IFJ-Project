//
// Created by Dominik Vagala on 28. 11. 2018.
//

#ifndef IFJ_PROJECT_CODE_GEN_H
#define IFJ_PROJECT_CODE_GEN_H

#include <stdio.h>


typedef struct {
    char *text;
    int alloc_size;
}Tstring_dynamic;

typedef struct code_list{

    char *instruct;
    char *operand_1;
    char *operand_2;
    char *operand_3;
//
//    Tstring_dynamic instruct;
//    Tstring_dynamic operand_1;
//    Tstring_dynamic operand_2;
//    Tstring_dynamic operand_3;

    struct code_list *next;
    struct code_list *start;
    struct code_list *end;

}*Tcode_list;


void print_code(Tcode_list code_list);
void add_code_line(Tcode_list *code_list, char *instruct, char *operand_1, char *operand_2, char *operand_3);
void init_code_list(Tcode_list *code_list);


char *make_name_for_GF(char *var_name_source, char *var_name_out);

#endif //IFJ_PROJECT_CODE_GEN_H


