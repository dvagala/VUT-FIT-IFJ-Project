/*
 * Project: Implementace překladače imperativního jazyka IFJ18
 *
 * File: code_gen.c
 *
 * xglosk00   Kristian Glos
 * xvagal00   Dominik Vagala
 * xvinsj00   Jakub Vins
 * xabike00   Zhamilya Abikenova
 *
 */

#include <stdlib.h>
#include "code_gen.h"
#include <string.h>

#define DEBGUG_CODE_GEN 0
#define DEBGUG_CODE_GEN_FREE 0
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
//            printf("\n");                       // This will nicely separate LABELS
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

    if(functions_code_list == NULL)
        return;

    active_code_list = functions_code_list->start;

    while(active_code_list != NULL){
        if(DEBGUG_CODE_GEN) if(DEBGUG_CODE_GEN) printf("CODE_GEN: freeint: %p\n", active_code_list);
        if(DEBGUG_CODE_GEN) if(DEBGUG_CODE_GEN) printf("CODE_GEN: freeint text: %p\n", active_code_list->text);

        if(active_code_list->next == NULL){
            if(DEBGUG_CODE_GEN) printf("Code gen: freeing string: %p\n", active_code_list);
            if(DEBGUG_CODE_GEN) printf("Code gen: freeing strin->text: %p\n", active_code_list->text);
            if(DEBGUG_CODE_GEN) printf("Code gen: freeing strin->text: %s\n", active_code_list->text);
            free(active_code_list->text);
            active_code_list->text = NULL;
            free(active_code_list);
            active_code_list = NULL;
            // Now go out of while
        } else{
            active_code_list = active_code_list->next;

            if(DEBGUG_CODE_GEN) printf("Code gen: freeing string: %p\n", active_code_list->prev);
            if(DEBGUG_CODE_GEN) printf("Code gen: freeing strin->text: %p\n", active_code_list->prev->text);
            if(DEBGUG_CODE_GEN) printf("Code gen: freeing strin->text: %s\n", active_code_list->prev->text);

            free(active_code_list->prev->text);
            active_code_list->prev->text = NULL;
            free(active_code_list->prev);
            active_code_list->prev = NULL;
        }
    }

    if(main_code_list == NULL)
        return;

    active_code_list = main_code_list->start;

    while(active_code_list != NULL){
        if(DEBGUG_CODE_GEN) if(DEBGUG_CODE_GEN) printf("CODE_GEN: freeint: %p\n", active_code_list);
        if(DEBGUG_CODE_GEN) if(DEBGUG_CODE_GEN) printf("CODE_GEN: freeint text: %p\n", active_code_list->text);

        if(active_code_list->next == NULL){

            if(DEBGUG_CODE_GEN) printf("Code gen: freeing string: %p\n", active_code_list);
            if(DEBGUG_CODE_GEN) printf("Code gen: freeing strin->text: %p\n", active_code_list->text);
            if(DEBGUG_CODE_GEN) printf("Code gen: freeing strin->text: %s\n", active_code_list->text);

            free(active_code_list->text);
            active_code_list->text = NULL;
            free(active_code_list);
            active_code_list = NULL;
            // Now go out of while
        } else{
            active_code_list = active_code_list->next;

            if(DEBGUG_CODE_GEN) printf("Code gen: freeing string: %p\n", active_code_list->prev);
            if(DEBGUG_CODE_GEN) printf("Code gen: freeing strin->text: %p\n", active_code_list->prev->text);
            if(DEBGUG_CODE_GEN) printf("Code gen: freeing strin->text: %s\n", active_code_list->prev->text);

            free(active_code_list->prev->text);
            active_code_list->prev->text = NULL;
            free(active_code_list->prev);
            active_code_list->prev = NULL;
        }
    }

    main_code_list = NULL;
    functions_code_list = NULL;
}

/**Appends unique number to the end of active_code_list */
void append_unique_code(){
    char *value = malloc(sizeof(int)+1);
    sprintf(value,"%d",generic_label_count);
    append_text_to_specific_string(active_code_list->end, value);
    free(value);
}

/**Appends unique number right behind good place for defvar */
void append_unique_code_to_newfound_defvar(){
    char *value = malloc(sizeof(int)+1);
    sprintf(value,"%d",generic_label_count);
    append_text_to_specific_string(find_nearest_good_place_for_defvar()->prev, value);
    free(value);
}

/**Generates operation depending on the passed operator. And adds unique number. */
void gen_unique_operation(Prec_table_symbols_enum operator,P_item *o1){
    char *value=malloc(sizeof(char)*8+1);
    switch (operator) {
        case P_PLUS:
            strcpy(value,"$plus");
            break;
        case P_MINUS:
            strcpy(value,"$subs");
            break;
        case P_MUL:
            strcpy(value,"$muls");
            break;
        case P_DIV:
            if(o1->operator == P_FLOAT_NUM) {
                strcpy(value, "$divs");
            }else if(o1->operator == P_INT_NUM){
                strcpy(value,"$idivs");
            }
            break;
        case P_LESS:
            strcpy(value,"$less");
            break;
        case P_EQ:
            strcpy(value,"$eqs");
            break;
        case P_MORE:
            strcpy(value,"$mores");
            break;
        case P_NOT_EQ:
            strcpy(value,"$noteqs");
            break;
        case P_LESS_EQ:
            strcpy(value, "$lesseqs");
            break;
        case P_MORE_EQ:
            strcpy(value,"$moreeqs");
            break;
        default:
            break;
    }
    add_string_after_specific_string(active_code_list->end,value);
    append_unique_code();
    free(value);

}

/** Generates res type depending if its for label or not and appends unique code.*/
void add_unique_res_type(bool label){
    if(label) {
        add_string_after_specific_string(active_code_list->end, "$%res$type");   // $%res$type$
    }
    else add_string_after_specific_string(active_code_list->end, "LF@%res$type");
    append_unique_code();
}

/**Generates var type and appends unique code. */
void add_unique_var_type(P_item *o1){
    item_value_gen_and_add(o1,false);
    append_text_to_specific_string(active_code_list->end,"$type"); // int$type1
    append_unique_code();
}

/**Generates LF@%res and appends unique code */
void add_unique_res(){
    add_string_after_specific_string(active_code_list->end, "LF@%res");
    append_unique_code();
}

/**Generates DEFVAR restype, adds it on the right place if is_while is true */
void declare_defvar_restype(bool is_while){
    if(is_while){
        add_string_after_specific_string(find_nearest_good_place_for_defvar()->prev,"DEFVAR");
        find_nearest_good_place_for_defvar()->prev->is_start_of_new_line = true;
        add_string_after_specific_string(find_nearest_good_place_for_defvar()->prev, "LF@%res$type");
        append_unique_code_to_newfound_defvar();
    }
    else {
        insert_simple_instruction("DEFVAR");
        add_unique_res_type(false);
    }
}

/**Generates DEFVAR res, adds it on the right place if is_while is true */
void insert_defvar_res(bool is_while){
    if(is_while){
        add_string_after_specific_string(find_nearest_good_place_for_defvar()->prev,"DEFVAR");
        find_nearest_good_place_for_defvar()->prev->is_start_of_new_line = true;
        add_string_after_specific_string(find_nearest_good_place_for_defvar()->prev, "LF@%res");
        append_unique_code_to_newfound_defvar();
    }else {
        insert_simple_instruction("DEFVAR");
        add_unique_res();
    }
}

/**Generates instruction to the code_list and defines it as the start of new line */
void insert_simple_instruction(char *instruction){
    add_string_after_specific_string(active_code_list->end,instruction);
    active_code_list->end->is_start_of_new_line = true;
}

/**Generates PUSHS res and appends unique code*/
void push_res(){
    insert_simple_instruction("PUSHS");
    add_string_after_specific_string(active_code_list->end, "LF@%res");
    append_unique_code();
}

/**Generates unique $end$ */
void add_end(){
    add_string_after_specific_string(active_code_list->end, "$end$");
    append_unique_code();
}

void add_exito(){
    add_string_after_specific_string(active_code_list->end, "$exito$");
    append_unique_code();
}

/**Generates needed exit code */
void exit_gen(int error_code){
    insert_simple_instruction("EXIT");
    add_string_after_specific_string(active_code_list->end,"int@");
    char *value = malloc(sizeof(int)+1);
    sprintf(value,"%d",error_code);
    append_text_to_specific_string(active_code_list->end, value);
    free(value);
}

/**Generates items value and adds it or appends it to the code */
void item_value_gen_and_add(P_item *item,bool append){
    char *value;
    char *ifj18string;
    switch(item->operator){
        case P_INT_NUM:
            value = malloc(sizeof(int)*4 +1);
            if(!append) {
                add_string_after_specific_string(active_code_list->end, "int@");
            }else append_text_to_specific_string(active_code_list->end, "int$");

            sprintf(value,"%d",item->value_int);
            append_text_to_specific_string(active_code_list->end,value);

            free(value);

            break;
        case P_FLOAT_NUM:
            value = malloc(sizeof(double)*4 +1);
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
            ifj18string = convert_string_to_correct_IFJcode18_format(value);
            append_text_to_specific_string(active_code_list->end,ifj18string);
            free(value);
            free(ifj18string);
            break;
        case P_ID:
            value = malloc(sizeof(strlen(item->string))+1);
            strcpy(value, item->string);
            if(!append){
                add_string_after_specific_string(active_code_list->end,"LF@");
            }else append_text_to_specific_string(active_code_list->end, "LF$");
            ifj18string = convert_string_to_correct_IFJcode18_format(value);
            append_text_to_specific_string(active_code_list->end,ifj18string);
            free(value);
            free(ifj18string);
            break;
        default:
            break;
    }

}

/**Generates unique label depending on operands type */
void gen_unique_label_for_res(P_item *o1){

    if(o1->operator == P_FLOAT_NUM){
        add_unique_res_type(true);
        append_text_to_specific_string(active_code_list->end, "$");   // $%res$type0$
        append_text_to_specific_string(active_code_list->end,"float");              //$%res$type$0float

    }else if (o1->operator == P_INT_NUM){
        add_unique_res_type(true);
        append_text_to_specific_string(active_code_list->end, "$");   // $%res$type0$
        append_text_to_specific_string(active_code_list->end,"int");

    }  else if (o1->operator == P_STRING){
        add_unique_res_type(true);
        append_text_to_specific_string(active_code_list->end, "$");   // $%res$type$
        append_text_to_specific_string(active_code_list->end,"string");
    }  else if(o1->operator == P_ID){
        add_unique_res_type(true);
        append_text_to_specific_string(active_code_list->end, "$");   // $%res$type$
        item_value_gen_and_add(o1,true);
    }


}

/**Generates Jumpifeq condition basically*/
void insert_res_and_type(P_item *o1){
    if (o1->operator == P_FLOAT_NUM) {
        gen_unique_label_for_res(o1);
        add_unique_res_type(false);
        add_string_after_specific_string(active_code_list->end, "string@float");
    } else if (o1->operator == P_INT_NUM) {
        gen_unique_label_for_res(o1);
        add_unique_res_type(false);
        add_string_after_specific_string(active_code_list->end, "string@int");// JUMPIFEQ $%res$type$0int $%res$type string@int
    } else if (o1->operator == P_STRING) {
        gen_unique_label_for_res(o1);
        add_unique_res_type(false);
        add_string_after_specific_string(active_code_list->end,
                                         "string@string");// JUMPIFEQ $%res$type$0string string@string
    } else if (o1->operator == P_ID){
        gen_unique_label_for_res(o1);
        add_unique_res_type(false);
        add_unique_var_type(o1);
    }

}

/**Adds or appends items type */
void item_type_gen_and_add(P_item *item, bool append){

    switch(item->operator) {
        case P_INT_NUM:
            if(!append)add_string_after_specific_string(active_code_list->end,"string@int");
            else append_text_to_specific_string(active_code_list->end,"$int");
            break;
        case P_FLOAT_NUM:
            if(!append)add_string_after_specific_string(active_code_list->end,"string@float");
            else append_text_to_specific_string(active_code_list->end,"$float");
            break;
        case P_STRING:
            if(!append)add_string_after_specific_string(active_code_list->end,"string@string");
            else append_text_to_specific_string(active_code_list->end,"$string");
            break;
        default:
            break;

    }

}

/**Generates custom jumpifeq */
void gen_custom_jumpifeq(P_item *o1, Prec_table_symbols_enum operator){
    insert_simple_instruction("JUMPIFEQ");
    gen_unique_operation(operator,o1);
    add_unique_res_type(false);
    item_type_gen_and_add(o1,false); //JUMPIFEQ $plus1 LF@%res$type1 string@int

}

/**Generates defvar if expression is in while cycle */
void gen_defvar_in_while(P_item *o1){
    add_string_after_specific_string(find_nearest_good_place_for_defvar()->prev,"DEFVAR");
    find_nearest_good_place_for_defvar()->prev->is_start_of_new_line = true;
    char *value = malloc(sizeof(strlen(o1->string))+1);
    char *ifj18string;
    strcpy(value, o1->string);
    add_string_after_specific_string(find_nearest_good_place_for_defvar()->prev,"LF@");
    ifj18string = convert_string_to_correct_IFJcode18_format(value);
    append_text_to_specific_string(find_nearest_good_place_for_defvar()->prev,ifj18string);
    free(ifj18string);
    free(value);
    append_text_to_specific_string(find_nearest_good_place_for_defvar()->prev,"$type");
    append_unique_code_to_newfound_defvar();

}

/**Insert any other instructions depending on operands or on special type */
void insert_instruction(char *instruction, P_item *o1, P_item *o2,char *type){
    add_string_after_specific_string(active_code_list->end,instruction);
    active_code_list->end->is_start_of_new_line = true;

    if(!strcmp(instruction,"PUSHS")){
        if(DEBUG_EXPR_GEN) printf("%s\n","I'm gonna push");
        if(o1->res){
            add_unique_res();
        }
        else item_value_gen_and_add(o1,false);
    }
    else if(!strcmp(instruction, "POPS")){
        add_string_after_specific_string(active_code_list->end,"LF@%res");
        append_unique_code();
    }
    else if(!strcmp(instruction,"CONCAT")){
        if(o2!=NULL) {
            add_string_after_specific_string(active_code_list->end, "LF@%res");
            append_unique_code();
            item_value_gen_and_add(o1, false);
            item_value_gen_and_add(o2, false);
        }
        else {
            add_string_after_specific_string(active_code_list->end, "LF@%res");
            append_unique_code();
            add_string_after_specific_string(active_code_list->end, "LF@%res");
            append_unique_code();
            item_value_gen_and_add(o1, false);

        }

    }
    else if(!strcmp(instruction, "MOVE")){
        if(o2 == NULL) {
            add_string_after_specific_string(active_code_list->end, "LF@%res");
            append_unique_code();
            item_value_gen_and_add(o1,false);
        }
    }
    else if(!strcmp(instruction, "TYPE")){
        if(o2 == NULL && o1 == NULL){
            add_unique_res_type(false);
            add_string_after_specific_string(active_code_list->end, "LF@%res");
            append_unique_code();
        }else{
            add_unique_var_type(o1);
            item_value_gen_and_add(o1,false);
        }

    }
    else if(!strcmp(instruction, "JUMPIFEQ")){
        if(type!=NULL) {
            if (!strcmp(type, "exito") && o2!=NULL) {
                add_exito();
                item_value_gen_and_add(o1, false);
                item_value_gen_and_add(o2, false);
            }else
            {
                add_exito();
                add_unique_res();
                item_value_gen_and_add(o1,false);
            }
        }
        else insert_res_and_type(o1);
    }
    else if(!strcmp(instruction, "LABEL")){

        if (type != NULL) {
            if (!strcmp(type, "end")) {
                add_end();
            }
            else if (!strcmp(type, "exito")) {
                add_exito();
            }
        } else gen_unique_label_for_res(o1);

    }
    else if(!strcmp(instruction, "JUMP")){
        if(!strcmp(type,"end")){
            add_end();
        }
    }
    else if(!strcmp(instruction,"DEFVAR")){
        add_unique_var_type(o1);

    }
    else if(!strcmp(instruction,"JUMPIFEQS")){
        if (type != NULL) {
            if (!strcmp(type, "exito")) {
                add_exito();
                item_value_gen_and_add(o1,false);
            }
        }
    }

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
                if(output != old_output){
//                    fprintf(stderr, "Reall\n");
                    free(old_output);
                }

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
                        if(output != old_output){
//                            fprintf(stderr, "Reall\n");
                            free(old_output);
                        }
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
            if(output != old_output){
//                fprintf(stderr, "Reall\n");
                free(old_output);
            }
        }
    }
    output[index]='\0';
    return output;
}

void generate_inputs_func(){
    add_string_after_specific_string(active_code_list->end, "LABEL inputs \n"
                                                            "PUSHFRAME \n"
                                                            "DEFVAR LF@%retval \n"
                                                            "MOVE LF@%retval nil@nil \n"
                                                            "READ LF@%retval string \n"
                                                            "POPFRAME \n"
                                                            "RETURN \n");
    active_code_list->end->is_start_of_new_line = true;
}

void generate_inputi_func(){
    add_string_after_specific_string(active_code_list->end, "LABEL inputi \n"
                                                            "PUSHFRAME \n"
                                                            "DEFVAR LF@%retval \n"
                                                            "MOVE LF@%retval nil@nil \n"
                                                            "READ LF@%retval int \n"
                                                            "POPFRAME \n"
                                                            "RETURN \n");
    active_code_list->end->is_start_of_new_line = true;
}

void generate_inputf_func(){
    add_string_after_specific_string(active_code_list->end, "LABEL inputf \n"
                                                            "PUSHFRAME \n"
                                                            "DEFVAR LF@%retval \n"
                                                            "MOVE LF@%retval nil@nil \n"
                                                            "READ LF@%retval float \n"
                                                            "POPFRAME \n"
                                                            "RETURN \n");
    active_code_list->end->is_start_of_new_line = true;
}

void generate_length_func(){

    add_string_after_specific_string(active_code_list->end, "LABEL length \n"
                                                            "PUSHFRAME \n"
                                                            "DEFVAR LF@%retval \n"
                                                            "MOVE LF@%retval nil@nil \n"
                                                            "DEFVAR LF@id_type \n"
                                                            "TYPE LF@id_type LF@s \n"
                                                            "JUMPIFNEQ $$length_continue0 LF@id_type string@nil\n"
                                                            "EXIT int@4\n"
                                                            "LABEL $$length_continue0    # must be string\n"
                                                            "JUMPIFEQ $$length_ok LF@id_type string@string \n"
                                                            "EXIT int@4 \n"
                                                            "LABEL $$length_ok \n"
                                                            "STRLEN LF@%retval LF@s \n"
                                                            "POPFRAME \n"
                                                            "RETURN \n");
    active_code_list->end->is_start_of_new_line = true;
}

void generate_substr_func(){
    add_string_after_specific_string(active_code_list->end, "LABEL substr \n"
                                                            "PUSHFRAME \n"
                                                            "DEFVAR LF@%retval \n"
                                                            "MOVE LF@%retval string@ \n"
                                                            "DEFVAR LF@new_char \n"
                                                            "DEFVAR LF@id_type \n"
                                                            "TYPE LF@id_type LF@s     \n"
                                                            "JUMPIFNEQ $$substr_continue0 LF@id_type string@nil\n"
                                                            "EXIT int@4\n"
                                                            "LABEL $$substr_continue0    # must be string\n"
                                                            "JUMPIFEQ $$substr_continue LF@id_type string@string\n"
                                                            "EXIT int@4\n"
                                                            "LABEL $$substr_continue\n"
                                                            "TYPE LF@id_type LF@i     \n"
                                                            "JUMPIFNEQ $$substr_continue1 LF@id_type string@nil\n"
                                                            "EXIT int@4\n"
                                                            "LABEL $$substr_continue1    # i cannot be string\n"
                                                            "JUMPIFNEQ $$substr_continue2 LF@id_type string@string\n"
                                                            "EXIT int@4\n"
                                                            "LABEL $$substr_continue2\n"
                                                            "TYPE LF@id_type LF@n     \n"
                                                            "JUMPIFNEQ $$substr_continue3 LF@id_type string@nil\n"
                                                            "EXIT int@4\n"
                                                            "LABEL $$substr_continue3    # n cannot be string\n"
                                                            "JUMPIFNEQ $$substr_continue4 LF@id_type string@string\n"
                                                            "EXIT int@4\n"
                                                            "LABEL $$substr_continue4\n"
                                                            "TYPE LF@id_type LF@i\n"
                                                            "JUMPIFNEQ $$substr_type_ok1 LF@id_type string@float \n"
                                                            "FLOAT2INT LF@i LF@i  # now have int of i\n"
                                                            "LABEL $$substr_type_ok1\n"
                                                            "TYPE LF@id_type LF@n\n"
                                                            "JUMPIFNEQ $$substr_type_ok2 LF@id_type string@float \n"
                                                            "FLOAT2INT LF@n LF@n  # now have int of n\n"
                                                            "LABEL $$substr_type_ok2\n"
                                                            "DEFVAR LF@$$substr_bounds\n"
                                                            "LT LF@$$substr_bounds LF@i int@0     # i check lower bound\n"
                                                            "JUMPIFEQ $$substr_end LF@$$substr_bounds bool@true\n"
                                                            "LT LF@$$substr_bounds LF@n int@0     # n check lower bound\n"
                                                            "JUMPIFEQ $$substr_end LF@$$substr_bounds bool@true\n"
                                                            "CREATEFRAME \n"
                                                            "DEFVAR TF@s \n"
                                                            "MOVE TF@s LF@s \n"
                                                            "CALL length \n"
                                                            "LT LF@$$substr_bounds LF@i TF@%retval    # i check higher bound\n"
                                                            "JUMPIFEQ $$substr_end LF@$$substr_bounds bool@false\n"
                                                            "LT LF@$$substr_bounds LF@n TF@%retval    # check if n > lenght(s) - 1, if yes set it to lenght(s) - 1\n"
                                                            "JUMPIFEQ $$substr_while LF@$$substr_bounds bool@true\n"
                                                            "SUB TF@%retval TF@%retval int@1\n"
                                                            "MOVE LF@n TF@%retval\n"
                                                            "LABEL $$substr_while \n"
                                                            "GETCHAR LF@new_char LF@s LF@i \n"
                                                            "CONCAT LF@%retval LF@%retval LF@new_char \n"
                                                            "JUMPIFEQ $$substr_end LF@i LF@n \n"
                                                            "ADD LF@i LF@i int@1 \n"
                                                            "JUMP $$substr_while \n"
                                                            "LABEL $$substr_end \n"
                                                            "POPFRAME \n"
                                                            "RETURN  ");
    active_code_list->end->is_start_of_new_line = true;
}

void generate_ord_func(){

    add_string_after_specific_string(active_code_list->end, "LABEL ord \n"
                                                            "PUSHFRAME \n"
                                                            "DEFVAR LF@%retval \n"
                                                            "MOVE LF@%retval nil@nil \n"
                                                            "DEFVAR LF@id_type \n"
                                                            "TYPE LF@id_type LF@s     \n"
                                                            "JUMPIFNEQ $$ord_continue0 LF@id_type string@nil\n"
                                                            "EXIT int@4\n"
                                                            "LABEL $$ord_continue0    # must be string\n"
                                                            "JUMPIFEQ $$ord_continue LF@id_type string@string\n"
                                                            "EXIT int@4\n"
                                                            "LABEL $$ord_continue\n"
                                                            "TYPE LF@id_type LF@i     \n"
                                                            "JUMPIFNEQ $$ord_continue1 LF@id_type string@nil\n"
                                                            "EXIT int@4\n"
                                                            "LABEL $$ord_continue1   # cannot be string\n"
                                                            "JUMPIFNEQ $$ord_continue2 LF@id_type string@string\n"
                                                            "EXIT int@4\n"
                                                            "LABEL $$ord_continue2\n"
                                                            "JUMPIFNEQ $$ord_type_ok LF@id_type string@float \n"
                                                            "FLOAT2INT LF@i LF@i  # now have int\n"
                                                            "LABEL $$ord_type_ok\n"
                                                            "DEFVAR LF@$$ord_bounds\n"
                                                            "LT LF@$$ord_bounds LF@i int@0     # check lower bound\n"
                                                            "JUMPIFEQ $$ord_end LF@$$ord_bounds bool@true\n"
                                                            "CREATEFRAME \n"
                                                            "DEFVAR TF@s \n"
                                                            "MOVE TF@s LF@s \n"
                                                            "CALL length \n"
                                                            "LT LF@$$ord_bounds LF@i TF@%retval    # check higher bound\n"
                                                            "JUMPIFEQ $$ord_end LF@$$ord_bounds bool@false\n"
                                                            "STRI2INT LF@%retval LF@s LF@i \n"
                                                            "LABEL $$ord_end  \n"
                                                            "POPFRAME \n"
                                                            "RETURN \n");
    active_code_list->end->is_start_of_new_line = true;
}

void generate_chr_func(){
    add_string_after_specific_string(active_code_list->end, "LABEL chr\n"
                                                            "PUSHFRAME \n"
                                                            "DEFVAR LF@%retval \n"
                                                            "MOVE LF@%retval nil@nil \n"
                                                            "DEFVAR LF@id_type \n"
                                                            "TYPE LF@id_type LF@i\n"
                                                            "JUMPIFNEQ $$chr_continue0 LF@id_type string@nil\n"
                                                            "EXIT int@4\n"
                                                            "LABEL $$chr_continue0\n"
                                                            "JUMPIFNEQ $$chr_continue LF@id_type string@string\n"
                                                            "EXIT int@4\n"
                                                            "LABEL $$chr_continue\n"
                                                            "JUMPIFNEQ $$chr_type_ok LF@id_type string@float \n"
                                                            "PUSHS LF@i\n"
                                                            "FLOAT2INTS\n"
                                                            "POPS LF@i   # now have int\n"
                                                            "LABEL $$chr_type_ok\n"
                                                            "DEFVAR LF@$$char_bounds\n"
                                                            "LT LF@$$char_bounds LF@i int@0\n"
                                                            "JUMPIFNEQ $$char_bound_ok_1 LF@$$char_bounds bool@true\n"
                                                            "EXIT int@4\n"
                                                            "LABEL $$char_bound_ok_1\n"
                                                            "GT LF@$$char_bounds LF@i int@255\n"
                                                            "JUMPIFNEQ $$char_bound_ok_2 LF@$$char_bounds bool@true\n"
                                                            "EXIT int@4\n"
                                                            "LABEL $$char_bound_ok_2\n"
                                                            "INT2CHAR LF@%retval LF@i \n"
                                                            "POPFRAME \n"
                                                            "RETURN \n");
    active_code_list->end->is_start_of_new_line = true;
}

void generate_system_functions(){

    add_string_after_specific_string(active_code_list->end, "\n# Start declaring system function");
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