//
// Created by Dominik Vagala on 29. 11. 2018.
//

#include "string_dynamic.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

void add_string_to_list(char* text){

    // First time
    if(string_list == NULL){
        string_list = malloc(sizeof(struct Sstring));
        string_list->start = string_list;
        string_list->end = string_list;

        string_list->text = text;

        string_list->next = NULL;
        string_list->prev = NULL;

        return;
    }

    string_list->end->next = malloc(sizeof(struct Sstring));
    string_list->end->next->prev = string_list->end;
    string_list->end->next->text = text;
    string_list->end->next->next = NULL;
    string_list->end = string_list->end->next;
}


char *append_to_string(char *text){

    Tstring string = string_list->end;
    char *old_text = string_list->end->text;
    printf("old: %p\n", old_text);

    char *appended = malloc(sizeof(char) * (strlen(string->text) + strlen(text)));
    strcpy(appended, string->text);
    strcat(appended, text);

    string->text = appended;

    printf("new: %p\n", appended);

    free(old_text);

    return appended;

}

void print_list_of_strings(){
    Tstring temp = string_list->start;

    while(temp != NULL){

//        printf("String-t pointer: %p\n", temp);
//        printf("String->text pointer: %p\n", temp->text);
        printf("String->text text: %s\n", temp->text);
        temp = temp->next;
    }
}

void print_list_of_strings_back(){
    Tstring temp = string_list->end;

    while(temp != NULL){
//        printf("String-t pointer: %p\n", temp);
//        printf("String->text pointer: %p\n", temp->text);
        printf("String->text text: %s\n", temp->text);
        temp = temp->prev;
    }
}

void free_list_of_strings(){

    while(string_list != NULL){

        printf("freeint: %p\n", string_list);
        printf("freeint text: %p\n", string_list->text);

        if(string_list->next == NULL){
            free(string_list);
            free(string_list->text);
            return;
        }

        string_list = string_list->next;

        free(string_list->prev);
        free(string_list->prev->text);
    }

    printf("end free\n");
}

