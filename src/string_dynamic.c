//
// Created by Dominik Vagala on 29. 11. 2018.
//

#include "string_dynamic.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Tstring_pile_list string_pile_list = NULL;

void create_string(Tstring *string, char *text){
    string->allocated_space = 0;
    string->text = NULL;
    string->free_space = 0;

    append_to_string(string, text);

//    add_string_to_pile_list(string);
}

void append_to_string(Tstring *string, char *text){

    printf("append_to_string, in: %s \n", string->text);

    // First time
    if(string->allocated_space == 0){
        string->allocated_space += (strlen(text) + 1);
        string->text = malloc( string->allocated_space * sizeof(char));
        strcpy(string->text, text);
        string->free_space = 0;
    } else if(string->free_space == 0){     // If full
        printf("strlen of string->text: %d\n", strlen(string->text));
        char *old_string_text_pointer = string->text;
        string->allocated_space += (strlen(text) + 1);
//        string->text = realloc(string->text, string->allocated_space * sizeof(char));
        string->text = malloc(string->allocated_space * sizeof(char));            // Just simulate scanario as if realloc change its address

        if(old_string_text_pointer != string->text){
            printf("Realloc change address, updating...\n");
            strcpy(string->text, old_string_text_pointer);
            free(old_string_text_pointer);
        }

        strcat(string->text, text);
        string->free_space = 0;
    }

    printf("append_to_string, out: %s \n", string->text);
 }

void replace_string_in_pile_list(Tstring *old_string, Tstring *new_string){
    Tstring_pile_list temp = string_pile_list->start;
    Tstring_pile_list previous = temp;

    int i = 0;

    printf("=============\n");

    while(temp != NULL){

        printf("param pint: :%p\n", *old_string);

        if(temp->string.text == old_string->text){
            printf("nooow\n");

            temp->next;
            previous->next->string = *new_string;

        }

        printf("actual pint: :%p\n", temp->string);


        previous = temp;
        temp = temp->next;

        i++;
    }

    printf("=============\n");
}

void print_pile_of_strings(){
    Tstring_pile_list temp = string_pile_list->start;
    Tstring_pile_list previous = temp;

    while(temp != NULL){

        printf("String->text pointer: %p\n", temp->string.text);
        printf("String->text text: %s\n", temp->string.text);
        temp = temp->next;
    }
}

void print_pile_of_strings_back(){
    Tstring_pile_list temp = string_pile_list->end;
    Tstring_pile_list previous = temp;

    while(temp != NULL){

        printf("String->text pointer: %p\n", temp->string.text);
        printf("String->text text: %s\n", temp->string.text);
        temp = temp->prev;
    }
}

void add_string_to_pile_list(Tstring *string){

    // First time
    if(string_pile_list == NULL){
        printf("first timen\n");
        string_pile_list = malloc(sizeof(struct Sstring_pile_list));
        string_pile_list->start = string_pile_list;
        string_pile_list->end = string_pile_list;

        string_pile_list->string = *string;

        string_pile_list->next = NULL;
        string_pile_list->prev = NULL;

        return;
    }

    printf("second timen\n");
    string_pile_list->end->next = malloc(sizeof(struct Sstring_pile_list));
    string_pile_list->end->next->prev = string_pile_list->end;
    string_pile_list->end->next->string = *string;
    string_pile_list->end->next->next = NULL;
    string_pile_list->end = string_pile_list->end->next;

}

//void remove_string_from_pile(Tstring *string){
//    Tstring_pile_list temp = string_pile_list->start;
//    Tstring_pile_list previous;
//
//    while(temp != NULL){
//        previous = temp;
//        temp = temp->next;
//        printf("Freeing :%s\n", previous->string.text);
//        free(previous->string.text);
//        free(previous);
//    }
//}

void free_pile_of_strings(){
    Tstring_pile_list temp = string_pile_list->start;
    Tstring_pile_list previous;

    while(temp != NULL){
        previous = temp;
        temp = temp->next;
        printf("Freeing :%s\n", previous->string.text);
        free(previous->string.text);
        free(previous);
    }
}
