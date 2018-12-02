//
// Created by Dominik Vagala on 2. 12. 2018.
//

#include "system_func_generate.h"
#include "code_gen.h"

void generate_print_func(){

}

void generate_inputs_func(){

}

void generate_inputi_func(){

}

void generate_inputf_func(){

}

void generate_lenght_func(){

}

void generate_substr_func(){

}

// id = ord(s, i)
// STRI2INT LF@id LF@s LF@i
void generate_ord_func(char *assign_var, char *var_s, char *var_i, char *s, int *i){

    //TODO type checks

    add_string_after_specific_string(active_code_list->end, "STRI2INT");
    active_code_list->end->is_start_of_new_line = true;

    add_string_after_specific_string(active_code_list->end, "LF@");
    active_code_list->end->is_start_of_new_line = true;


}

void generate_chr_func(){

}