//
// Created by Dominik Vagala on 28. 11. 2018.
//

#include <stdlib.h>
#include "code_gen.h"

void print_code(Tcode_list code_list){

    Tcode_list temp = code_list;

    while(temp != NULL){

        printf("%s ", temp->instruct);
        if(temp->operand_1 != NULL)
            printf("%s ", temp->operand_1);
        if(temp->operand_2 != NULL)
            printf("%s ", temp->operand_2);
        if(temp->operand_3 != NULL)
            printf("%s ", temp->operand_3);

        printf("\n");

        temp = temp->next;
    }

}

void add_line(Tcode_list *code_list, char *instruct, char *operand_1, char *operand_2, char *operand_3){

    // First time
    if((*code_list) == NULL){
        (*code_list) = malloc(sizeof(struct code_list));
        (*code_list)->start = (*code_list);
        (*code_list)->end = (*code_list);

        (*code_list)->instruct = instruct;
        (*code_list)->operand_1 = operand_1;
        (*code_list)->operand_2 = operand_2;
        (*code_list)->operand_3 = operand_3;

        (*code_list)->next = NULL;

        return;
    }

    (*code_list)->end->next = malloc(sizeof(struct code_list));

    (*code_list)->end->next->instruct = instruct;
    (*code_list)->end->next->operand_1 = operand_1;
    (*code_list)->end->next->operand_2 = operand_2;
    (*code_list)->end->next->operand_3 = operand_3;

    (*code_list)->end->next->next = NULL;

    (*code_list)->end = (*code_list)->next;

}

void init_code_list(Tcode_list *code_list){
    *code_list = NULL;
}

//int main(){
//
//    Tcode_list code_list;
//    code_list = NULL;
//
//    add_line(&code_list, "DEFVAR", "GF@counter", NULL, NULL);
//    add_line(&code_list, "MOVE", "GF@counter", "string@", NULL);
//    add_line(&code_list, "LABEL", "GF@counter", "string@", NULL);
//
//    print_code(code_list);
//
//    return 0;
//}