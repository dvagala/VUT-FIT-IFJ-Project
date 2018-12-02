//
// Created by Dominik Vagala on 28. 11. 2018.
//

#include <stdlib.h>
#include "code_gen.h"
#include <string.h>

#define DEBGUG_CODE_GEN 0
#define DEBUG_EXPR_GEN 0

void code_list_init(){
    active_code_list = NULL;
    main_code_list = NULL;
    functions_code_list = NULL;
}

/**Print code nicely formatted*/
void print_code(){
    if(DEBGUG_CODE_GEN) if(DEBGUG_CODE_GEN) printf("CODE_GEN: printing code...\n");

//    if(active_code_list == main_code_list)
//        if(DEBGUG_CODE_GEN) printf("\nPrinting main code...\n");
//    else
//        if(DEBGUG_CODE_GEN) printf("\nPrinting func code...\n");

    if(active_code_list == NULL)
        return;

    Tstring temp = active_code_list = active_code_list->start;
    while(temp != NULL){
        if(temp->is_start_of_new_line)
            printf("\n");
        printf("%s ", temp->text);
        temp = temp->next;
    }
    printf("\n");
}

// Just for testing
void print_code_backwards(){
    if(DEBGUG_CODE_GEN) if(DEBGUG_CODE_GEN) printf("CODE_GEN: printing code...\n");

    if(DEBGUG_CODE_GEN) printf("\nPrinting code...\n");

    if(active_code_list == NULL)
        return;

    Tstring temp = active_code_list->end;
    while(temp != NULL){

        if(temp->is_start_of_new_line)
            if(DEBGUG_CODE_GEN) printf("\n");
        if(DEBGUG_CODE_GEN) printf("%s ", temp->text);
        temp = temp->prev;
    }
    if(DEBGUG_CODE_GEN) printf("\n");
}

/**Append text to specific string, I do not create new string as add_string_after_specific_string does*/
char* append_text_to_specific_string(Tstring specific_string, char *text){

    if(specific_string == NULL){
        if(DEBGUG_CODE_GEN) if(DEBGUG_CODE_GEN) printf("Specific_string == NULL!");
        exit(1);
    }

    char *old_text = specific_string->text;
    char *appended = malloc(sizeof(char) * ((strlen(specific_string->text) + strlen(text) + 1)));
    if(DEBGUG_CODE_GEN) printf("alocing appen text: %p\n", appended);
    strcpy(appended, specific_string->text);
    strcat(appended, text);

    specific_string->text = appended;
    free(old_text);

    return appended;
}

/**Create new string with text, located after string you pass me, I dont free that *text here*/
void add_string_after_specific_string(Tstring specific_string, char *text){

    struct Sstring *new_string = malloc(sizeof(struct Sstring));
    if(DEBGUG_CODE_GEN) printf("allocing string: %p\n", new_string);
    new_string->text = malloc(sizeof(char)*(strlen(text) + 1));
    if(DEBGUG_CODE_GEN) printf("alocing string->text: %p\n", new_string->text);

    strcpy(new_string->text, text);
    new_string->is_start_of_new_line = false;
    new_string->before_me_is_good_place_for_defvar = false;

    // First time
    if(active_code_list == NULL){
        new_string->prev = NULL;
        active_code_list = new_string;
        active_code_list->start = new_string;
        new_string->next = NULL;
        active_code_list->end = new_string;
    }else if(specific_string == NULL){              // Insert at the beggining of the active_code_list
        if(DEBGUG_CODE_GEN) printf("great\n");
        new_string->prev = NULL;
        new_string->next = active_code_list->start;
        active_code_list->start->prev = new_string;
        active_code_list->start = new_string;
    }else if(active_code_list->end == specific_string){    // instert at the end of active_code_list
        new_string->next = NULL;
        new_string->prev = active_code_list->end;
        active_code_list->end->next = new_string;
        active_code_list->end = new_string;
    }else{                                          // Insert somewhere in the middle
        new_string->prev = specific_string;
        new_string->next = specific_string->next;
        specific_string->next = new_string;
        new_string->next->prev = new_string;
    }

    if(DEBGUG_CODE_GEN) printf("alocing string->text: %s\n", new_string->text);

    if(specific_string == main_code_list && functions_code_list != NULL)
        main_code_list = active_code_list;

}

// Return string what is before string that has "before_me_is_good_place_for_defvar" true
Tstring find_nearest_good_place_for_defvar(){
    Tstring temp = active_code_list->end;

    while(temp != NULL){
        if(temp->before_me_is_good_place_for_defvar){
            if(DEBGUG_CODE_GEN) if(DEBGUG_CODE_GEN) printf("Before me is good place: %p, %s\n", temp, temp->text);
            return temp->prev;
        }
        temp = temp->prev;
    }
    if(DEBGUG_CODE_GEN) if(DEBGUG_CODE_GEN) printf("not found good place for defvar\n");
    return NULL;
}

void free_code_lists(){

    active_code_list = functions_code_list->start;

    while(active_code_list != NULL){
        if(DEBGUG_CODE_GEN) if(DEBGUG_CODE_GEN) printf("CODE_GEN: freeint: %p\n", active_code_list);
        if(DEBGUG_CODE_GEN) if(DEBGUG_CODE_GEN) printf("CODE_GEN: freeint text: %p\n", active_code_list->text);

        if(active_code_list->next == NULL){
            if(DEBGUG_CODE_GEN) printf("freeing string: %p\n", active_code_list);
            if(DEBGUG_CODE_GEN) printf("freeing strin->text: %p\n", active_code_list->text);
            if(DEBGUG_CODE_GEN) printf("freeing strin->text: %s\n", active_code_list->text);
            free(active_code_list->text);
            active_code_list->text = NULL;
            free(active_code_list);
            active_code_list = NULL;
            // Now go out of while
        } else{
            active_code_list = active_code_list->next;

            if(DEBGUG_CODE_GEN) printf("freeing string: %p\n", active_code_list->prev);
            if(DEBGUG_CODE_GEN) printf("freeing strin->text: %p\n", active_code_list->prev->text);
            if(DEBGUG_CODE_GEN) printf("freeing strin->text: %s\n", active_code_list->prev->text);

            free(active_code_list->prev->text);
            active_code_list->prev->text = NULL;
            free(active_code_list->prev);
            active_code_list->prev = NULL;
        }
    }

    active_code_list = main_code_list->start;

    while(active_code_list != NULL){
        if(DEBGUG_CODE_GEN) if(DEBGUG_CODE_GEN) printf("CODE_GEN: freeint: %p\n", active_code_list);
        if(DEBGUG_CODE_GEN) if(DEBGUG_CODE_GEN) printf("CODE_GEN: freeint text: %p\n", active_code_list->text);

        if(active_code_list->next == NULL){

            if(DEBGUG_CODE_GEN) printf("freeing string: %p\n", active_code_list);
            if(DEBGUG_CODE_GEN) printf("freeing strin->text: %p\n", active_code_list->text);
            if(DEBGUG_CODE_GEN) printf("freeing strin->text: %s\n", active_code_list->text);

            free(active_code_list->text);
            active_code_list->text = NULL;
            free(active_code_list);
            active_code_list = NULL;
            // Now go out of while
        } else{
            active_code_list = active_code_list->next;

            if(DEBGUG_CODE_GEN) printf("freeing string: %p\n", active_code_list->prev);
            if(DEBGUG_CODE_GEN) printf("freeing strin->text: %p\n", active_code_list->prev->text);
            if(DEBGUG_CODE_GEN) printf("freeing strin->text: %s\n", active_code_list->prev->text);

            free(active_code_list->prev->text);
            active_code_list->prev->text = NULL;
            free(active_code_list->prev);
            active_code_list->prev = NULL;
        }
    }

    main_code_list = NULL;
    functions_code_list = NULL;
}


bool declare_defvar_restype(){
    insert_simple_instruction("DEFVAR");
    add_string_after_specific_string(active_code_list->end, "LF@%res$type");
}

bool insert_defvar_res(){
    insert_simple_instruction("DEFVAR");
    add_string_after_specific_string(active_code_list->end, "LF@%res");
}

bool insert_simple_instruction(char *instruction){
    add_string_after_specific_string(active_code_list->end,instruction);
    active_code_list->end->is_start_of_new_line = true;
    return true;
}

bool insert_instruction(char *instruction, P_item *o1, P_item *o2,P_stack *post_stack){
    add_string_after_specific_string(active_code_list->end,instruction);
    active_code_list->end->is_start_of_new_line = true;
    if(!strcmp(instruction,"PUSHS")){
        if(DEBUG_EXPR_GEN) printf("%s\n","I'm gonna push");
        top_value_gen_and_add(post_stack);
    }
    else if(!strcmp(instruction, "POPS")){
        add_string_after_specific_string(active_code_list->end,"LF@%res");
    }
    else if(!strcmp(instruction,"CONCAT")){
        add_string_after_specific_string(active_code_list->end,"LF@%res");
        top_value_gen_and_add(post_stack);
        top_value_gen_and_add(post_stack);

    }
    else if(!strcmp(instruction, "MOVE")){
        if(o2 == NULL) {
            add_string_after_specific_string(active_code_list->end, "LF@%res");
            top_value_gen_and_add(post_stack);
        }
    }
    else if(!strcmp(instruction, "TYPE")){
        if(o2 == NULL && o1 == NULL){
            add_string_after_specific_string(active_code_list->end, "LF@%res$type");
            add_string_after_specific_string(active_code_list->end, "LF@%res");
        }
    }

    return true;
}

bool top_value_gen_and_add(P_stack *post_stack){
    char *value;
    if(!post_stack)
        return false;
    switch(post_stack->top->operator){
        case P_INT_NUM:
            value = malloc(sizeof(int)*4 +1);
            if(!value)
                return false;
            add_string_after_specific_string(active_code_list->end,"int@");
            sprintf(value,"%d",post_stack->top->value_int);
            append_text_to_specific_string(active_code_list->end,value);

            free(value);

            break;
        case P_FLOAT_NUM:
            value = malloc(sizeof(double)*4 +1);
            if(!value)
                return false;
            add_string_after_specific_string(active_code_list->end,"float@");
            sprintf(value,"%a",post_stack->top->value_double);
            append_text_to_specific_string(active_code_list->end,value);
            free(value);
            break;
        case P_STRING:
            value = malloc(sizeof(strlen(post_stack->top->string))+1);
            strcpy(value, post_stack->top->string);
            add_string_after_specific_string(active_code_list->end,"string@");
            append_text_to_specific_string(active_code_list->end,value);
            free(value);
            break;
        case P_ID:
            value = malloc(sizeof(strlen(post_stack->top->string))+1);
            strcpy(value, post_stack->top->string);
            add_string_after_specific_string(active_code_list->end,"LF@");
            append_text_to_specific_string(active_code_list->end,value);
            free(value);
            break;
        default:
            return false;
    }
    p_stack_pop(post_stack);

//    if(DEBUG_EXPR_GEN)printf("%s%d\n","first in queue is:",q.first->operator);
    return true;
}



/**Convert input to this format:
 * string@retezec\032s\032lomitkem\032\092\032a\010novym\035radkem
 * */
char* convert_string_to_correct_IFJcode18_format(char *input)
{
    int chunk=50;
    int amount=1;
    char number[3];
    int index=0;
    char* output=malloc(sizeof(char)*chunk*amount);
    if(DEBGUG_CODE_GEN) printf("mallocing: %p\n", output);

    for (int i=0;input[i]!='\0';i++) {
        if ((input[i]>=0 && input[i]<=32) || input[i]==35 || input[i]==92) {
            if(DEBGUG_CODE_GEN) printf(number,"%d",input[i]);
            output[index]='\\';
            index++;

            if (index==chunk*amount||index+1==chunk*amount||index+2==chunk*amount) {
                amount++;
                char *old_output = output;
                output=realloc(output, sizeof(char)*chunk*amount);
                // If realloc returned different address, free that old one
                if(output != old_output)
                    free(old_output);
            }

            output[index]='0';
            index++;

            if (number[1]=='\0') {
                output[index]='0';
                index++;
                output[index]=number[0];
                index++;
            }
            else {
                for (int j=0;j<2;j++) {
                    if (index==chunk*amount) {
                        amount++;
                        char *old_output = output;
                        output=realloc(output, sizeof(char)*chunk*amount);
                        // If realloc returned different address, free that old one
                        if(output != old_output)
                            free(old_output);
                    }
                    output[index]=number[j];
                    index++;

                }
            }
        }
        else {
            output[index]=input[i];
            index++;
        }
        if (index==chunk*amount) {
            amount++;
            char *old_output = output;
            output=realloc(output, sizeof(char)*chunk*amount);
            // If realloc returned different address, free that old one
            if(output != old_output)
                free(old_output);
        }
    }
    output[index]='\0';
    return output;
}