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

    if(active_code_list == main_code_list)
        if(DEBGUG_CODE_GEN) printf("\nPrinting main code...\n");
    else
        if(DEBGUG_CODE_GEN) printf("\nPrinting func code...\n");

    if(active_code_list == NULL)
        return;

    Tstring temp = active_code_list = active_code_list->start;
    while(temp != NULL){
        if(temp->is_start_of_new_line)
            printf("\n");
        if(strcmp(temp->text, "LABEL") == 0){
            printf("\n");                       // This will nicely separate LABELS
            if(temp->next != NULL){
                if(temp->next->text != NULL){
                    if(temp->next->text[0] != '$')
                        printf("\n");      // This will nicely separate functions, their names doesnt start with $
                }
            }
        }
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
bool push_res(){
    insert_simple_instruction("PUSHS");
    add_string_after_specific_string(active_code_list->end, "LF@%res");
    return true;
}


bool item_type_gen_and_add(P_item *item){
    char *value;
    if(!item)
        return false;
    switch(item->operator) {
        case P_INT_NUM:
            add_string_after_specific_string(active_code_list->end,"string@int");
            break;
        case P_FLOAT_NUM:
            add_string_after_specific_string(active_code_list->end,"string@float");
            break;
        case P_STRING:
            add_string_after_specific_string(active_code_list->end,"string@string");
            break;
        default: return false;

    }
    return true;
}

bool item_value_gen_and_add(P_item *item,bool append){
    char *value;
    if(!item)
        return false;
    switch(item->operator){
        case P_INT_NUM:
            value = malloc(sizeof(int)*4 +1);
            if(!value)
                return false;
            if(!append) {
                add_string_after_specific_string(active_code_list->end, "int@");
            }else append_text_to_specific_string(active_code_list->end, "int$");

            sprintf(value,"%d",item->value_int);
            append_text_to_specific_string(active_code_list->end,value);

            free(value);

            break;
        case P_FLOAT_NUM:
            value = malloc(sizeof(double)*4 +1);
            if(!value)
                return false;
            if(!append) {
                add_string_after_specific_string(active_code_list->end, "float@");
            }else append_text_to_specific_string(active_code_list->end, "float$");
            sprintf(value,"%a",item->value_double);
            append_text_to_specific_string(active_code_list->end,value);
            free(value);
            break;
        case P_STRING:
            value = malloc(sizeof(strlen(item->string))+1);
            strcpy(value, item->string);
            if(!append){
                add_string_after_specific_string(active_code_list->end,"string@");
            }else append_text_to_specific_string(active_code_list->end, "string$");
            append_text_to_specific_string(active_code_list->end,convert_string_to_correct_IFJcode18_format(value));
            free(value);
            break;
        case P_ID:
            value = malloc(sizeof(strlen(item->string))+1);
            strcpy(value, item->string);
            if(!append){
                add_string_after_specific_string(active_code_list->end,"LF@");
            }else append_text_to_specific_string(active_code_list->end, "LF$");
            append_text_to_specific_string(active_code_list->end,convert_string_to_correct_IFJcode18_format(value));
            free(value);
            break;
        default:
            return false;
    }

//    if(DEBUG_EXPR_GEN)printf("%s%d\n","first in queue is:",q.first->operator);
    return true;
}

bool insert_instruction(char *instruction, P_item *o1, P_item *o2){
    add_string_after_specific_string(active_code_list->end,instruction);
    active_code_list->end->is_start_of_new_line = true;
    if(!strcmp(instruction,"PUSHS")){
        if(DEBUG_EXPR_GEN) printf("%s\n","I'm gonna push");
        item_value_gen_and_add(o1,false);
    }
    else if(!strcmp(instruction, "POPS")){
        add_string_after_specific_string(active_code_list->end,"LF@%res");
    }
    else if(!strcmp(instruction,"CONCAT")){
        add_string_after_specific_string(active_code_list->end,"LF@%res");
        item_value_gen_and_add(o1,false);
        item_value_gen_and_add(o1,false);

    }
    else if(!strcmp(instruction, "MOVE")){
        if(o2 == NULL) {
            add_string_after_specific_string(active_code_list->end, "LF@%res");
            item_value_gen_and_add(o1,false);
        }
    }
    else if(!strcmp(instruction, "TYPE")){
        if(o2 == NULL && o1 == NULL){
            add_string_after_specific_string(active_code_list->end, "LF@%res$type");
            add_string_after_specific_string(active_code_list->end, "LF@%res");
        }
    }
    else if(!strcmp(instruction, "JUMPIFEQ")){
        add_string_after_specific_string(active_code_list->end, "$%res$type$");
        item_value_gen_and_add(o1,true);
        add_string_after_specific_string(active_code_list->end, "LF@%res$type");
        item_type_gen_and_add(o1);
    }
    else if(!strcmp(instruction, "LABEL")){
        add_string_after_specific_string(active_code_list->end, "$%res$type$");
        item_value_gen_and_add(o1,true);
    }

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

    for (int i=0;input[i]!='\0';i++) {
        if ((input[i]>=0 && input[i]<=32) || input[i]==35 || input[i]==92) {
            sprintf(number,"%d",input[i]);
            output[index]='\\';
            index++;

            if (index==chunk*amount||index+1==chunk*amount||index+2==chunk*amount) {
                amount++;
                char *old_output = output;
                output=realloc(output, sizeof(char)*chunk*amount);
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

//TODO: type checks

void generate_inputs_func(){
    add_string_after_specific_string(active_code_list->end, "LABEL");
    active_code_list->end->is_start_of_new_line = true;
    add_string_after_specific_string(active_code_list->end, "inputs");
    add_string_after_specific_string(active_code_list->end, "PUSHFRAME");
    active_code_list->end->is_start_of_new_line = true;

    add_string_after_specific_string(active_code_list->end, "DEFVAR");
    active_code_list->end->is_start_of_new_line = true;
    add_string_after_specific_string(active_code_list->end, "LF@%retval");
    add_string_after_specific_string(active_code_list->end, "MOVE");
    active_code_list->end->is_start_of_new_line = true;
    add_string_after_specific_string(active_code_list->end, "LF@%retval");
    add_string_after_specific_string(active_code_list->end, "nil@nil");

    add_string_after_specific_string(active_code_list->end, "READ");
    active_code_list->end->is_start_of_new_line = true;
    add_string_after_specific_string(active_code_list->end, "LF@%retval");
    add_string_after_specific_string(active_code_list->end, "string");

    add_string_after_specific_string(active_code_list->end, "POPFRAME");
    active_code_list->end->is_start_of_new_line = true;
    add_string_after_specific_string(active_code_list->end, "RETURN");
    active_code_list->end->is_start_of_new_line = true;
}

void generate_inputi_func(){
    add_string_after_specific_string(active_code_list->end, "LABEL");
    active_code_list->end->is_start_of_new_line = true;
    add_string_after_specific_string(active_code_list->end, "inputi");
    add_string_after_specific_string(active_code_list->end, "PUSHFRAME");
    active_code_list->end->is_start_of_new_line = true;

    add_string_after_specific_string(active_code_list->end, "DEFVAR");
    active_code_list->end->is_start_of_new_line = true;
    add_string_after_specific_string(active_code_list->end, "LF@%retval");
    add_string_after_specific_string(active_code_list->end, "MOVE");
    active_code_list->end->is_start_of_new_line = true;
    add_string_after_specific_string(active_code_list->end, "LF@%retval");
    add_string_after_specific_string(active_code_list->end, "nil@nil");

    add_string_after_specific_string(active_code_list->end, "READ");
    active_code_list->end->is_start_of_new_line = true;
    add_string_after_specific_string(active_code_list->end, "LF@%retval");
    add_string_after_specific_string(active_code_list->end, "int");

    add_string_after_specific_string(active_code_list->end, "POPFRAME");
    active_code_list->end->is_start_of_new_line = true;
    add_string_after_specific_string(active_code_list->end, "RETURN");
    active_code_list->end->is_start_of_new_line = true;
}

void generate_inputf_func(){
    add_string_after_specific_string(active_code_list->end, "LABEL");
    active_code_list->end->is_start_of_new_line = true;
    add_string_after_specific_string(active_code_list->end, "inputf");
    add_string_after_specific_string(active_code_list->end, "PUSHFRAME");
    active_code_list->end->is_start_of_new_line = true;

    add_string_after_specific_string(active_code_list->end, "DEFVAR");
    active_code_list->end->is_start_of_new_line = true;
    add_string_after_specific_string(active_code_list->end, "LF@%retval");
    add_string_after_specific_string(active_code_list->end, "MOVE");
    active_code_list->end->is_start_of_new_line = true;
    add_string_after_specific_string(active_code_list->end, "LF@%retval");
    add_string_after_specific_string(active_code_list->end, "nil@nil");

    add_string_after_specific_string(active_code_list->end, "READ");
    active_code_list->end->is_start_of_new_line = true;
    add_string_after_specific_string(active_code_list->end, "LF@%retval");
    add_string_after_specific_string(active_code_list->end, "float");

    add_string_after_specific_string(active_code_list->end, "POPFRAME");
    active_code_list->end->is_start_of_new_line = true;
    add_string_after_specific_string(active_code_list->end, "RETURN");
    active_code_list->end->is_start_of_new_line = true;
}

//id = length(s)
//STRLEN LF@id LF@s
//# LF@s must be string
void generate_length_func(){
    add_string_after_specific_string(active_code_list->end, "LABEL");
    active_code_list->end->is_start_of_new_line = true;
    add_string_after_specific_string(active_code_list->end, "length");
    add_string_after_specific_string(active_code_list->end, "PUSHFRAME");
    active_code_list->end->is_start_of_new_line = true;

    add_string_after_specific_string(active_code_list->end, "DEFVAR");
    active_code_list->end->is_start_of_new_line = true;
    add_string_after_specific_string(active_code_list->end, "LF@%retval");
    add_string_after_specific_string(active_code_list->end, "MOVE");
    active_code_list->end->is_start_of_new_line = true;
    add_string_after_specific_string(active_code_list->end, "LF@%retval");
    add_string_after_specific_string(active_code_list->end, "nil@nil");

    add_string_after_specific_string(active_code_list->end, "STRLEN");
    active_code_list->end->is_start_of_new_line = true;
    add_string_after_specific_string(active_code_list->end, "LF@%retval");
    add_string_after_specific_string(active_code_list->end, "LF@s");

    add_string_after_specific_string(active_code_list->end, "POPFRAME");
    active_code_list->end->is_start_of_new_line = true;
    add_string_after_specific_string(active_code_list->end, "RETURN");
    active_code_list->end->is_start_of_new_line = true;
}

//out = substr(s, i, n)
//# s must be string
//# i,n int
void generate_substr_func(){
    add_string_after_specific_string(active_code_list->end, "LABEL");
    active_code_list->end->is_start_of_new_line = true;
    add_string_after_specific_string(active_code_list->end, "substr");
    add_string_after_specific_string(active_code_list->end, "PUSHFRAME");
    active_code_list->end->is_start_of_new_line = true;

    add_string_after_specific_string(active_code_list->end, "DEFVAR");
    active_code_list->end->is_start_of_new_line = true;
    add_string_after_specific_string(active_code_list->end, "LF@%retval");
    add_string_after_specific_string(active_code_list->end, "MOVE");
    active_code_list->end->is_start_of_new_line = true;
    add_string_after_specific_string(active_code_list->end, "LF@%retval");
    add_string_after_specific_string(active_code_list->end, "string@");

    add_string_after_specific_string(active_code_list->end, "DEFVAR");
    active_code_list->end->is_start_of_new_line = true;
    add_string_after_specific_string(active_code_list->end, "LF@new_char");

    add_string_after_specific_string(active_code_list->end, "LABEL");
    active_code_list->end->is_start_of_new_line = true;
    add_string_after_specific_string(active_code_list->end, "$$substr_while");

    add_string_after_specific_string(active_code_list->end, "GETCHAR");
    active_code_list->end->is_start_of_new_line = true;
    add_string_after_specific_string(active_code_list->end, "LF@new_char");
    add_string_after_specific_string(active_code_list->end, "LF@s");
    add_string_after_specific_string(active_code_list->end, "LF@i");

    add_string_after_specific_string(active_code_list->end, "CONCAT");
    active_code_list->end->is_start_of_new_line = true;
    add_string_after_specific_string(active_code_list->end, "LF@%retval");
    add_string_after_specific_string(active_code_list->end, "LF@%retval");
    add_string_after_specific_string(active_code_list->end, "LF@new_char");

    add_string_after_specific_string(active_code_list->end, "JUMPIFEQ");
    active_code_list->end->is_start_of_new_line = true;
    add_string_after_specific_string(active_code_list->end, "$$substr_end");
    add_string_after_specific_string(active_code_list->end, "LF@i");
    add_string_after_specific_string(active_code_list->end, "LF@n");

    add_string_after_specific_string(active_code_list->end, "ADD");
    active_code_list->end->is_start_of_new_line = true;
    add_string_after_specific_string(active_code_list->end, "LF@i");
    add_string_after_specific_string(active_code_list->end, "LF@i");
    add_string_after_specific_string(active_code_list->end, "int@1");

    add_string_after_specific_string(active_code_list->end, "JUMP");
    active_code_list->end->is_start_of_new_line = true;
    add_string_after_specific_string(active_code_list->end, "$$substr_while");

    add_string_after_specific_string(active_code_list->end, "LABEL");
    active_code_list->end->is_start_of_new_line = true;
    add_string_after_specific_string(active_code_list->end, "$$substr_end");

    add_string_after_specific_string(active_code_list->end, "POPFRAME");
    active_code_list->end->is_start_of_new_line = true;
    add_string_after_specific_string(active_code_list->end, "RETURN");
    active_code_list->end->is_start_of_new_line = true;
}

// id = ord(s, i)
// STRI2INT LF@id LF@s LF@i
void generate_ord_func(){

    add_string_after_specific_string(active_code_list->end, "LABEL");
    active_code_list->end->is_start_of_new_line = true;
    add_string_after_specific_string(active_code_list->end, "ord");
    add_string_after_specific_string(active_code_list->end, "PUSHFRAME");
    active_code_list->end->is_start_of_new_line = true;

    add_string_after_specific_string(active_code_list->end, "DEFVAR");
    active_code_list->end->is_start_of_new_line = true;
    add_string_after_specific_string(active_code_list->end, "LF@%retval");
    add_string_after_specific_string(active_code_list->end, "MOVE");
    active_code_list->end->is_start_of_new_line = true;
    add_string_after_specific_string(active_code_list->end, "LF@%retval");
    add_string_after_specific_string(active_code_list->end, "nil@nil");

    add_string_after_specific_string(active_code_list->end, "STRI2INT");
    active_code_list->end->is_start_of_new_line = true;
    add_string_after_specific_string(active_code_list->end, "LF@%retval");
    add_string_after_specific_string(active_code_list->end, "LF@s");
    add_string_after_specific_string(active_code_list->end, "LF@i");

    add_string_after_specific_string(active_code_list->end, "POPFRAME");
    active_code_list->end->is_start_of_new_line = true;
    add_string_after_specific_string(active_code_list->end, "RETURN");
    active_code_list->end->is_start_of_new_line = true;
}

//id = chr(i)
//INT2CHAR LF@id LF@i
//# i is int <0, 255> if not error 58
//#
void generate_chr_func(){
    add_string_after_specific_string(active_code_list->end, "LABEL");
    active_code_list->end->is_start_of_new_line = true;
    add_string_after_specific_string(active_code_list->end, "chr");
    add_string_after_specific_string(active_code_list->end, "PUSHFRAME");
    active_code_list->end->is_start_of_new_line = true;

    add_string_after_specific_string(active_code_list->end, "DEFVAR");
    active_code_list->end->is_start_of_new_line = true;
    add_string_after_specific_string(active_code_list->end, "LF@%retval");
    add_string_after_specific_string(active_code_list->end, "MOVE");
    active_code_list->end->is_start_of_new_line = true;
    add_string_after_specific_string(active_code_list->end, "LF@%retval");
    add_string_after_specific_string(active_code_list->end, "nil@nil");

    add_string_after_specific_string(active_code_list->end, "INT2CHAR");
    active_code_list->end->is_start_of_new_line = true;
    add_string_after_specific_string(active_code_list->end, "LF@%retval");
    add_string_after_specific_string(active_code_list->end, "LF@i");

    add_string_after_specific_string(active_code_list->end, "POPFRAME");
    active_code_list->end->is_start_of_new_line = true;
    add_string_after_specific_string(active_code_list->end, "RETURN");
    active_code_list->end->is_start_of_new_line = true;
}

void generate_system_functions(){

    add_string_after_specific_string(active_code_list->end, "# Start declaring system function");
    active_code_list->end->is_start_of_new_line = true;

    generate_inputf_func();
    generate_inputi_func();
    generate_inputs_func();
    generate_ord_func();
    generate_chr_func();
    generate_length_func();
    generate_substr_func();

    add_string_after_specific_string(active_code_list->end, "# End declaring system function");
    active_code_list->end->is_start_of_new_line = true;

}