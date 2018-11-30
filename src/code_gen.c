//
// Created by Dominik Vagala on 28. 11. 2018.
//

#include <stdlib.h>
#include "code_gen.h"
#include <string.h>

#define DEBGUG_CODE_GEN 0

void code_list_init(){
    code_list = NULL;
}

void print_code(){

    if(DEBGUG_CODE_GEN) printf("CODE_GEN: printing code...\n");

    Tstring temp = code_list;

    while(temp != NULL){

        if(temp->is_start_of_new_line)
            printf("\n");
        printf("%s ", temp->text);
        temp = temp->next;
    }

    printf("\n");

}

char* append_text_to_last_string_in_code_list(char *text){

    Tstring end_string = code_list->end;
    char *old_text = code_list->end->text;
    if(DEBGUG_CODE_GEN) printf("CODE_GEN: Ive got: %p\n", old_text);
    if(DEBGUG_CODE_GEN) printf("CODE_GEN: Ive got: %s\n", old_text);

    char *appended = malloc(sizeof(char) * ((strlen(end_string->text) + strlen(text) + 1)));
    strcpy(appended, end_string->text);
    strcat(appended, text);

    end_string->text = appended;

    if(DEBGUG_CODE_GEN) printf("CODE_GEN: I send: %p\n", appended);
    if(DEBGUG_CODE_GEN) printf("CODE_GEN: I send: %s\n", appended);
    free(old_text);

    return appended;
}

// For "any_text", or for any char, that is not mallocated
void add_const_string_to_code(char *text){

    // First time
    if(code_list == NULL){
        code_list = malloc(sizeof(struct Sstring));
        code_list->start = code_list;
        code_list->end = code_list;

        code_list->text = malloc(sizeof(char) * (strlen(text) + 1));
        strcpy(code_list->text, text);
        code_list->next = NULL;
        code_list->prev = NULL;

        code_list->is_start_of_new_line = false;

        return;
    }

    code_list->end->next = malloc(sizeof(struct Sstring));
    code_list->end->next->prev = code_list->end;
    code_list->end->next->text = text;
    code_list->end->next->next = NULL;
    code_list->end = code_list->end->next;

    code_list->end->is_start_of_new_line = false;
}

// You only pass me pointer to char* that is already allocated, I handle all free stuff
void add_allocated_string_to_code(char *text){

    // First time
    if(code_list == NULL){
        code_list = malloc(sizeof(struct Sstring));
        code_list->start = code_list;
        code_list->end = code_list;

        code_list->text = text;

        code_list->next = NULL;
        code_list->prev = NULL;

        code_list->is_start_of_new_line = false;

        return;
    }

    code_list->end->next = malloc(sizeof(struct Sstring));
    code_list->end->next->prev = code_list->end;
    code_list->end->next->text = text;
    code_list->end->next->next = NULL;
    code_list->end = code_list->end->next;

    code_list->end->is_start_of_new_line = false;
}

void free_code_list(){

    while(code_list != NULL){

        if(DEBGUG_CODE_GEN) printf("CODE_GEN: freeint: %p\n", code_list);
        if(DEBGUG_CODE_GEN) printf("CODE_GEN: freeint text: %p\n", code_list->text);

        if(code_list->next == NULL){
            free(code_list);
            free(code_list->text);
            return;
        }

        code_list = code_list->next;

        free(code_list->prev);
        free(code_list->prev->text);
    }
}
