//
// Created by Dominik Vagala on 28. 11. 2018.
//

#ifndef IFJ_PROJECT_CODE_GEN_H
#define IFJ_PROJECT_CODE_GEN_H

#include <stdio.h>

typedef struct code_list{

    char *instruct;
    char *operand_1;
    char *operand_2;
    char *operand_3;

    struct code_list *next;
    struct code_list *start;
    struct code_list *end;

}*Tcode_list;

void print_code(Tcode_list code_list);
void add_line(Tcode_list *code_list, char *instruct, char *operand_1, char *operand_2, char *operand_3);
void init_code_list(Tcode_list *code_list);


#endif //IFJ_PROJECT_CODE_GEN_H


