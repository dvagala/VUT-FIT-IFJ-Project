/*
 * Project: Implementace překladače imperativního jazyka IFJ18
 *
 * File: parser.c
 *
 * xglosk00   Kristian Glos
 * xvagal00   Dominik Vagala
 * xvinsj00   Jakub Vins
 * xabike00   Zhamilya Abikenova
 *
 */

#include <math.h>
#include "parser.h"
#include "scanner.h"
#include "code_gen.h"
#include "expressions.h"

int error_code = 0;

tToken token;
tToken aheadToken;          // When I need to do lookahead what is after actual token, this is it, use for expression detect
tToken previousToken;       // always behind actual token, use for expression detect
bool tokenLookAheadFlag = false;

// If we are in definition of function this is local_symtable of that function, otherwise == global_symtable
Bnode actual_symtable;
Bnode global_symtable;

// Because I set token type as EXPR when I find out that is indeed start of expression, I need
// somehow store this original type of token, so it can set back to original before calling analyze_expression()
int original_token_type_backup;

/** Wrapper for nextToken enhanced with memory
 *  Always remember previously readed token
 *  If someone used lookahead for token, tokenLookAhead flag is raised
 *  so then do not read next token but return aheadToken*/
tToken enhanced_next_token(){

    previousToken = token;
    if(tokenLookAheadFlag){
        tokenLookAheadFlag = false;
        return aheadToken;
    } else{
        tToken next_token = nextToken();

//         Throw away multiplte eols
        if(previousToken.type == EOL_CASE && next_token.type == EOL_CASE){
            next_token = enhanced_next_token();
        }
        return next_token;
    }
}

tToken next_token_lookahead(){
    aheadToken = nextToken();
    tokenLookAheadFlag = true;

    return aheadToken;
}

/**
 *  id_var   =          id_var     +      5
 *           ^            ^        ^
 *    previous_token    token  aheadToken
 * */
bool is_token_start_of_expr() {

    if (token.type == IDENTIFICATOR) {
        if(previousToken.type == IF || previousToken.type == WHILE){
            if(is_func_defined(&global_symtable, token.data.string)){
                // This ensures that only first error will be in error_code
                if(error_code == 0)
                    error_code = 3;
            }
            else if(!is_id_variable(&actual_symtable, token.data.string)) {
                // This ensures that only first error will be in error_code
                if (error_code == 0)
                    error_code = 3;
            }
            return true;
        }
        aheadToken = next_token_lookahead();
        // This throw sematic error 2, when: func + 5
        if(is_func_defined(&global_symtable, token.data.string)){
            if(aheadToken.type == PLUS || aheadToken.type == MINUS || aheadToken.type == DIVIDE || aheadToken.type == MULT ||
            aheadToken.type == EQUAL || aheadToken.type == NOTEQUAL || aheadToken.type == LOE || aheadToken.type == MOE ||
            aheadToken.type == LESS || aheadToken.type == MORE){
                // This ensures that only first error will be in error_code
                if(error_code == 0)
                    error_code = 2;
            }
        }
        if(is_id_variable(&actual_symtable, token.data.string)){
            if(aheadToken.type != ASSIGN && previousToken.type != LPAR &&
               previousToken.type != COMMA && previousToken.type != IDENTIFICATOR){
                // There are only 3 cases when id_variable is not start of expression
                // 1. id_variable = restOfCode
                // 2. def id_func(id_variable, id_variable)
                // 3. def id_func id_variable, id_variable
                return true;
            }
        }
    } else if(token.type == INT || token.type == STRING || token.type == FLOAT || token.type == FLOAT_EXPO){
        // In this cases it is not start of expression
        // 1. id_func 45, "hoho"
        // 2. id_func (45, "hoho")
        if(previousToken.type != IDENTIFICATOR && previousToken.type != COMMA && previousToken.type != LPAR){
            return true;
        }
    } else if(token.type == LPAR && previousToken.type != IDENTIFICATOR){
        // There are only two cases when '(' is not start of expression
        // 1. id = id(id, id)
        // 2. def id(id, id)
        // So if token before '(' was id it is certainly not an expression

        return true;
    }
    return false;
}

/**Pop token out of the 'stack'*/
void pop(){

    token = enhanced_next_token();

    if(token.type == ERROR && error_code == 0){          // Check lex error
        error_code = 1;
        return;
    }

    if(is_token_start_of_expr()){
        original_token_type_backup = token.type;      // Store original token type, so later i can be reverted and handed to expression analyzer
        token.type = EXPR;                            // override token type, so we can handle it easily with rules
    }
}

/**----------RECURSIVE DESCENT------------------------------------*/

bool expr(){

    // Set back original token type
    token.type = original_token_type_backup;

    add_string_after_specific_string(active_code_list->end, "# Expr start");
    active_code_list->end->is_start_of_new_line = true;

    ReturnData *returnData;

    returnData = analyze_expresssion(token, aheadToken, tokenLookAheadFlag, &actual_symtable);

    token = *(returnData->token);

    if(error_code == 0 && returnData->error){
        error_code = returnData->error_code;
    }

    free(returnData);

    // Because after analyze_expression this will reset
    tokenLookAheadFlag = false;

    add_string_after_specific_string(active_code_list->end, "# Expr done, store result on stack");
    active_code_list->end->is_start_of_new_line = true;

    return true;
}

bool more_param(char *func_name){

    // 33. More_params -> eps
    if(token.type == RPAR){

        return true;
    } else if(token.type == COMMA){        // 32. More_params -> , id More_params
        pop();
        if(token.type == IDENTIFICATOR){

            // Semantics. Add var_name to func_name parameters, if var_name was already there error 3
            char *var_name = token.data.string;

            if(is_variable_already_in_func_params(&global_symtable, func_name, var_name)){
                // This ensures that only first error will be in error_code
                if(error_code == 0)
                    error_code = 3;
                return false;
            }

            if(!add_variable_to_func_params(&global_symtable, func_name, var_name)){
                // This ensures that only first error will be in error_code
                if(error_code == 0)
                    error_code = 99;
                return false;
            }


            if(!add_variable_to_symtable(&actual_symtable, var_name)){
                // This ensures that only first error will be in error_code
                if(error_code == 0)
                    error_code = 99;
                return false;
            }

            // Free string stored in token.data.string, dont need this anymore
            free(var_name);
            var_name = NULL;

            pop();
            return more_param(func_name);
        }
    }

    return false;
}

bool param(char *func_name){

    // 31. Param -> id More_params
    if(token.type == IDENTIFICATOR){

        // Semantics. Add var_name to func_name parameters, if var_name was already there error 3
        char *var_name = token.data.string;

        if(is_variable_already_in_func_params(&global_symtable, func_name, var_name)){
            // This ensures that only first error will be in error_code
            if(error_code == 0)
                error_code = 3;
            return false;
        }

        // return false when malloc fails
        if(!add_variable_to_func_params(&global_symtable, func_name, var_name)){
            // This ensures that only first error will be in error_code
            if(error_code == 0)
                error_code = 99;
            return false;
        }

        if(!add_variable_to_symtable(&actual_symtable, var_name)){
            // This ensures that only first error will be in error_code
            if(error_code == 0)
                error_code = 99;
            return false;
        }

        // Free string stored in token.data.string, dont need this anymore
        free(var_name);
        var_name = NULL;

        pop();
        return more_param(func_name);
    } else if(token.type == RPAR){        // 30. Param -> eps

        return true;
    }

    return false;
}

bool def_func(){

    // 27. Def_func -> def id ( Param ) eol St_list end eol
    if(token.type == DEF) {
        active_code_list = functions_code_list;     // Switch context
        pop();
        if (token.type == IDENTIFICATOR) {

            char *func_name = token.data.string;

            // Semantics. Check redefinition of func, if yes -> error 3
            // Semantics. Check if func_name isnt previously declared global variable, if yes -> error 3
            // Semantics. Add id_func to symtable, return false if malloc fail
            if(is_func_defined(&global_symtable, func_name)){
                // This ensures that only first error will be in error_code
                if(error_code == 0)
                    error_code = 3;
                free(func_name);
                return false;
            }
            if(has_func_same_name_as_global_variable(&global_symtable, func_name)){
                // This ensures that only first error will be in error_code
                if(error_code == 0)
                    error_code = 3;
                free(func_name);
                return false;
            }
            if(!add_func_to_symtable(&global_symtable, func_name)){
                // This ensures that only first error will be in error_code
                if(error_code == 0)
                    error_code = 99;
                free(func_name);
                return false;
            }

            // Generate code
            add_string_after_specific_string(active_code_list->end, "LABEL");
            active_code_list->end->is_start_of_new_line = true;
            add_string_after_specific_string(active_code_list->end, func_name);
            add_string_after_specific_string(active_code_list->end, "PUSHFRAME");
            active_code_list->end->is_start_of_new_line = true;
            add_string_after_specific_string(active_code_list->end, "DEFVAR LF@%retval");
            active_code_list->end->is_start_of_new_line = true;
            add_string_after_specific_string(active_code_list->end, "MOVE LF@%retval nil@nil");
            active_code_list->end->is_start_of_new_line = true;
            add_string_after_specific_string(active_code_list->end, "PUSHS nil@nil");
            active_code_list->end->is_start_of_new_line = true;

            // New local symtable for this function
            local_symtable_init(&actual_symtable);

            pop();
            if (token.type == LPAR) {
                pop();
                bool result = param(func_name);

                // Free string stored in token.data.string, dont need this anymore
                free(func_name);

                if(!result)
                    return false;
                if (token.type == RPAR) {
                    pop();
                    if (token.type == EOL_CASE) {
                        pop();
                        if(!st_list())
                            return false;
                        if (token.type == END) {

                            add_string_after_specific_string(active_code_list->end, "POPS LF@%retval");
                            active_code_list->end->is_start_of_new_line = true;

                            // Generate code
                            add_string_after_specific_string(active_code_list->end, "POPFRAME");
                            active_code_list->end->is_start_of_new_line = true;
                            add_string_after_specific_string(active_code_list->end, "RETURN");
                            active_code_list->end->is_start_of_new_line = true;

                            // Swithing context go global, as we go out of function
                            free_symtable(&actual_symtable);
                            actual_symtable = global_symtable;
                            active_code_list = main_code_list;
                            pop();
                            if (token.type == EOL_CASE) {
                                pop();

                                return true;
                            }
                        }
                    }
                }
            }
        }
    }

    return false;
}

bool more_args(int *num_of_args, char *func_name){

    // 17. More-args -> eps
    if(token.type == EOL_CASE || token.type == RPAR){

        return true;
    } else if(token.type == COMMA){         // 16. More-args -> , Term More-args
        pop();
        (*num_of_args)++;                   // So we found another argument
        return term(num_of_args, func_name) && more_args(num_of_args, func_name);
    }

    return false;
}

bool term(int *num_of_args, char *func_name){

    // Generate code
    if(strcmp(func_name, "print") != 0){
        add_string_after_specific_string(active_code_list->end, "DEFVAR TF@");
        active_code_list->end->is_start_of_new_line = true;
        char *defined_param_name = get_name_of_defined_param_at_position(&global_symtable, func_name, (*num_of_args)-1);
        if(defined_param_name == NULL)
            return false;
        append_text_to_specific_string(active_code_list->end, defined_param_name);
        add_string_after_specific_string(active_code_list->end, "MOVE TF@");
        active_code_list->end->is_start_of_new_line = true;
        append_text_to_specific_string(active_code_list->end, defined_param_name);
    } else{
        add_string_after_specific_string(active_code_list->end, "WRITE");
        active_code_list->end->is_start_of_new_line = true;
    }

    // 18. Term -> id
    if(token.type == IDENTIFICATOR){

        char *var_name = token.data.string;

        // Semantics. Check if var_name was defined as variable, if not error 3
        if(!is_variable_defined(&actual_symtable, var_name)){
            // This ensures that only first error will be in error_code
            if(error_code == 0)
                error_code = 3;
            return false;
        }

        if(is_func_defined(&global_symtable, var_name)){
            if(error_code == 0)
                error_code = 2;
            return false;
        }

        add_string_after_specific_string(active_code_list->end, "LF@");
        append_text_to_specific_string(active_code_list->end, var_name);

        // Free string stored in token.data.string, dont need this anymore
        free(var_name);
        var_name = NULL;

        pop();

        return true;
    } else if(token.type == INT){           // 19. Term -> int

        if(strcmp(func_name, "length") == 0){    // function lenght() cant have int as argument
            if(error_code == 0)
                error_code = 4;
            return false;
        } else if(strcmp(func_name, "chr") == 0  ){
            if(token.data.value_int < 0 || token.data.value_int > 255){     // Check if number is in correct interval
                if(error_code == 0)
                    error_code = 4;
                return false;
            }
        }else if((strcmp(func_name, "ord") == 0 || strcmp(func_name, "ord") == 0) && *num_of_args == 1){
            if(error_code == 0)
                error_code = 4;
            return false;
        }

        char str[12];       // 12 because 32bit int cant have higher value
        sprintf(str, "%d", token.data.value_int);

        add_string_after_specific_string(active_code_list->end, "int@");
        append_text_to_specific_string(active_code_list->end, str);

        pop();

        return true;
    } else if(token.type == FLOAT){         // 20. Term -> double

        if(strcmp(func_name, "length") == 0){    // function lenght() cant have float as argument
            if(error_code == 0)
                error_code = 4;
            return false;
        } else if(strcmp(func_name, "chr") == 0  ){
            if(trunc(token.data.value_double) < 0 || trunc(token.data.value_double) > 255){     // Check if number is in correct interval
                if(error_code == 0)
                    error_code = 4;
                return false;
            }
        }else if((strcmp(func_name, "ord") == 0 || strcmp(func_name, "substr") == 0) && *num_of_args == 1){
            if(error_code == 0)
                error_code = 4;
            return false;
        }

        // Generate code
        char str[24];
        sprintf(str,  "%a", token.data.value_double);
        add_string_after_specific_string(active_code_list->end, "float@");
        append_text_to_specific_string(active_code_list->end, str);

        pop();

        return true;
    } else if(token.type == STRING){        // 21. Term -> string

        if(strcmp(func_name, "chr") == 0){
            if(error_code == 0)
                error_code = 4;
            return false;
        }else if((strcmp(func_name, "ord") == 0 || strcmp(func_name, "substr") == 0) && (*num_of_args == 2 || *num_of_args == 3)){
            if(error_code == 0)
                error_code = 4;
            return false;
        }

        // Generate code
        add_string_after_specific_string(active_code_list->end, "string@");
        char *converted = convert_string_to_correct_IFJcode18_format(token.data.string);
        append_text_to_specific_string(active_code_list->end, converted);

        free(converted);
        free(token.data.string);

        pop();

        return true;
    } else if(token.type == NIL){           // 22. Term -> nil

        if(strcmp(func_name, "length") == 0 || strcmp(func_name, "chr") == 0 || strcmp(func_name, "ord") == 0 || strcmp(func_name, "substr") == 0){
            if(error_code == 0)
                error_code = 4;
            return false;
        }

        // Generate code
        add_string_after_specific_string(active_code_list->end, "nil@nil");

        pop();

        return true;
    }

    return false;
}

bool arg_in_brackets(int *num_of_args, char *func_name){

    // 15. Arg_in_brackets -> Term More-args
    if(token.type == IDENTIFICATOR || token.type == INT || token.type == FLOAT ||
       token.type == STRING || token.type == NIL){

        // '1' becase if we get here, there will be ceratily at least one term + more_arg
        *num_of_args = 1;

        return term(num_of_args, func_name) && more_args(num_of_args, func_name);
    } else if(token.type == RPAR){            // 14. Arg_in_brackets -> eps

        return true;
    }

    return false;
}

bool call_func_args(int *num_of_args, char *func_name){

    // Generate code
    if(strcmp(func_name, "print") == 0){

    }else {
        add_string_after_specific_string(active_code_list->end, "CREATEFRAME");
        active_code_list->end->is_start_of_new_line = true;
    }

    // 13. Call_func_args -> Term More-args
    if(token.type == IDENTIFICATOR || token.type == INT || token.type == FLOAT ||
       token.type == STRING || token.type == NIL){

        // '1' becase if we get here, there will be ceratily at least one term + more_arg
        *num_of_args = 1;

        bool result = term(num_of_args, func_name) && more_args(num_of_args, func_name);

        return result;
    } else if(token.type == EOL_CASE){            // 11. Call_func_args -> eps
        return true;
    } else if(token.type == LPAR){            // 12. Call_func_args -> ( Arg_in_brackets )
        pop();

        if(!arg_in_brackets(num_of_args, func_name))
            return false;
        if(token.type == RPAR){
            pop();
            return true;
        }
    }

    return false;
}

bool call_func(char *var_name, char **func_name_to_parent){

    // 8. Call_func -> id Call_func_args
    if(token.type == IDENTIFICATOR){

        char *func_name = token.data.string;
        (*func_name_to_parent) = func_name;
        pop();

        // Semantics. Check if func_name was defined before calling, if not error 3
        if(!is_func_defined(&global_symtable, func_name)){
            // This ensures that only first error will be in error_code
            if(error_code == 0)
                error_code = 3;
            free(func_name);
            return false;
        }

        int num_of_args = 0;
        // num_of_args is handed by pointer, so sub_functions can later increment its value if there is another argument
        bool sub_analysis_result = call_func_args(&num_of_args, func_name);

        // Semantics. Check if num_of_args == func_name.defined.parameters, if not error 5
        // If number of define_params == -1,
        // it means number of parameters is variable (for system func print()), thus skip error scope
        int defined_params = get_num_of_defined_func_params(&global_symtable, func_name);
        if(num_of_args != defined_params && defined_params != -1){
            // This ensures that only first error will be in error_code
            if(error_code == 0)
                error_code = 5;
            free(func_name);
            return false;
        }

        // Generate code
        if(strcmp(func_name, "print") == 0){

        } else {
            add_string_after_specific_string(active_code_list->end, "CALL");
            active_code_list->end->is_start_of_new_line = true;
            add_string_after_specific_string(active_code_list->end, func_name);
        }

        return sub_analysis_result;
    }

    return false;
}

bool func_or_expr(char *var_name, char **func_name_to_parent){

    // 7. Func_or_expr -> Call_func
    if(token.type == IDENTIFICATOR){
        char *func_name = NULL;
        bool result =  call_func(var_name, &func_name);

        // Pass func_name to parent
        (*func_name_to_parent) = func_name;

        return result;

    } else if(token.type == EXPR){      // 9. Func_or_expr -> Expr
        bool result = expr();

        return result;
    }


    return false;
}

bool after_id() {

    // 10. After_id -> Call_func_args
    if(token.type == IDENTIFICATOR || token.type == EOL_CASE || token.type == LPAR ||
       token.type == INT || token.type == FLOAT || token.type == STRING || token.type == NIL){

        // previous_token == id_func      // cause we are in after_id
        char *func_name = previousToken.data.string;

        // Semantics. Check if func_name was defined before calling, if not error 3
        if(!is_func_defined(&global_symtable, func_name)){
            // This ensures that only first error will be in error_code
            if(error_code == 0)
                error_code = 3;
            free(func_name);
            return false;
        }

        int num_of_args = 0;
        // num_of_args is handed by pointer, so sub_functions can later increment its value if there is another argument
        bool sub_analysis_result = call_func_args(&num_of_args, func_name);

        // Semantics. Check if num_of_args == func_name.defined.parameters, if not error 5
        // If number of define_params == -1,
        // it means number of parameters is variable (at leaast 1), (for system func print()), thus skip error scope
        int defined_params = get_num_of_defined_func_params(&global_symtable, func_name);
        if(num_of_args != defined_params && (defined_params != -1 || num_of_args < 1)){
            // This ensures that only first error will be in error_code
            if(error_code == 0)
                error_code = 5;
            free(func_name);
            return false;
        }

        // Generate code
        if(strcmp(func_name, "print") != 0){
            add_string_after_specific_string(active_code_list->end, "CALL");
            active_code_list->end->is_start_of_new_line = true;
            add_string_after_specific_string(active_code_list->end, func_name);

            // If this is last line in func, put result of expression on stack
            add_string_after_specific_string(active_code_list->end, "PUSHS TF@%retval");
            active_code_list->end->is_start_of_new_line = true;
        }

        // Free string stored in token.data.string, dont need this anymore
        free(func_name);
        func_name = NULL;

        return sub_analysis_result;
    }else if(token.type == ASSIGN){        // 6. After_id -> = Func_or_expr

        // previous_token == id_var      // cause we are in after_id
        char *var_name = previousToken.data.string;

        // If name of variable ends with '!' or '?' throw error, only functions cant ends like this
        if(var_name[strlen(var_name)-1] == '?' || var_name[strlen(var_name)-1] == '!'){
            // This ensures that only first error will be in error_code
            if(error_code == 0)
                error_code = 2;
            free(var_name);
            return false;
        }

        // Semantics. Check if var_name isnt previously declared function -> error 3
        // Semantics. Add var_name to symtable if not already
        if(has_variable_same_name_as_func(&global_symtable, var_name)){
            // This ensures that only first error will be in error_code
            if(error_code == 0)
                error_code = 3;
            free(var_name);
            return false;
        }

        bool is_new_variable = false;
        if(!is_variable_defined(&actual_symtable, var_name))
            is_new_variable = true;

        // return false when malloc fails
        if(!add_variable_to_symtable(&actual_symtable, var_name)){
            // This ensures that only first error will be in error_code
            if(error_code == 0)
                error_code = 99;
            free(var_name);
            return false;
        }

        pop();

        char *func_name = NULL;

        // Generate code
        // if var_name where we want assign return value from function is new variable, define it
        if(is_new_variable){
            Tstring good_place_for_defvar = active_code_list->end;
            if(im_in_while_or_if)
                good_place_for_defvar = find_nearest_good_place_for_defvar()->prev;

            add_string_after_specific_string(good_place_for_defvar, "DEFVAR LF@");
            good_place_for_defvar->next->is_start_of_new_line = true;
            append_text_to_specific_string(good_place_for_defvar->next, var_name);
            add_string_after_specific_string(good_place_for_defvar->next, "MOVE LF@");
            good_place_for_defvar->next->next->is_start_of_new_line = true;
            append_text_to_specific_string(good_place_for_defvar->next->next, var_name);
            add_string_after_specific_string(good_place_for_defvar->next->next, "nil@nil");
        }

        bool result = func_or_expr(var_name, &func_name);

        if(func_name != NULL) {
            if(strcmp(func_name, "print") != 0) {
                add_string_after_specific_string(active_code_list->end, "MOVE");
                active_code_list->end->is_start_of_new_line = true;
                add_string_after_specific_string(active_code_list->end, "LF@");
                append_text_to_specific_string(active_code_list->end, var_name);
                add_string_after_specific_string(active_code_list->end, "TF@%retval");
            }
        } else{ // So it was expression
            add_string_after_specific_string(active_code_list->end, "POPS");
            active_code_list->end->is_start_of_new_line = true;
            add_string_after_specific_string(active_code_list->end, "LF@");
            append_text_to_specific_string(active_code_list->end, var_name);
        }

        // If this is last line in func, put result of expression on stack
        add_string_after_specific_string(active_code_list->end, "PUSHS LF@");
        active_code_list->end->is_start_of_new_line = true;
        append_text_to_specific_string(active_code_list->end, var_name);

        // Free string stored in token.data.string, dont need this anymore
        free(var_name);
        var_name = NULL;

        // Free string stored in token.data.string, dont need this anymore
        free(func_name);
        func_name = NULL;


        return result;
    }

    return false;
}

bool st_list() {

    // 28. St_list -> State St_list
    if (token.type == IDENTIFICATOR || token.type == EXPR || token.type == IF || token.type == WHILE) {
        return state() && st_list();
    } else if (token.type == ELSE || token.type == END) {      // 29. St_list -> eps
        return true;
    }

    return false;
}

bool state(){

    // 5. State -> id After_id eol
    if(token.type == IDENTIFICATOR ){
        pop();
        if(!after_id())
            return false;
        if(token.type == EOL_CASE ) {
            pop();

            return true;
        }
    } else if(token.type == EXPR){      // 24. State -> Expr eol
        if(!expr())
            return false;
        if(token.type == EOL_CASE ) {
            pop();

            return true;
        }
    } else if(token.type == IF){        // 25. State -> if Expr then eol St_list else eol St_list end eol
        pop();

        // For nested while loops or if statement
        bool im_parent_while_or_if = false;

        char str[12];       // 12 because 32bit int cant have higher value
        int condition_num = generic_label_count++;

        // Place DEFVAR at good place, if we are in while loop or if statement, above that label
        Tstring good_place_for_defvar = active_code_list->end;
        if(im_in_while_or_if)
            good_place_for_defvar = find_nearest_good_place_for_defvar()->prev;

        add_string_after_specific_string(good_place_for_defvar, "DEFVAR LF@condition_");
        good_place_for_defvar->next->is_start_of_new_line = true;
        sprintf(str, "%d", condition_num);
        append_text_to_specific_string(good_place_for_defvar->next, str);

        if(!expr())
            return false;

        add_string_after_specific_string(active_code_list->end, "POPS LF@condition_");
        active_code_list->end->is_start_of_new_line = true;
        sprintf(str, "%d", condition_num);
        append_text_to_specific_string(active_code_list->end, str);

        add_string_after_specific_string(active_code_list->end, "JUMPIFNEQ $else_");
        active_code_list->end->is_start_of_new_line = true;
        int else_num = generic_label_count++;
        sprintf(str, "%d", else_num );
        append_text_to_specific_string(active_code_list->end, str);
        append_text_to_specific_string(active_code_list->end, " LF@condition_");
        sprintf(str, "%d", condition_num);
        append_text_to_specific_string(active_code_list->end, str);
        append_text_to_specific_string(active_code_list->end, " bool@true");

        if(!im_in_while_or_if){
            active_code_list->end->before_me_is_good_place_for_defvar = true;
            im_parent_while_or_if = true;
        }
        im_in_while_or_if = true;

        if(token.type == THEN ) {
            pop();
            if(token.type == EOL_CASE ) {
                pop();
                if(!st_list())
                    return false;

                add_string_after_specific_string(active_code_list->end, "JUMP $if_end_");
                active_code_list->end->is_start_of_new_line = true;
                int if_end_num = generic_label_count++;
                sprintf(str, "%d", if_end_num);
                append_text_to_specific_string(active_code_list->end, str);

                if(token.type == ELSE ) {
                    pop();

                    add_string_after_specific_string(active_code_list->end, "LABEL $else_");
                    active_code_list->end->is_start_of_new_line = true;
                    sprintf(str, "%d", else_num);
                    append_text_to_specific_string(active_code_list->end, str);

                    if(token.type == EOL_CASE ) {
                        pop();
                        if(!st_list())
                            return false;
                        if(token.type == END ) {
                            pop();
                            if(token.type == EOL_CASE ) {
                                pop();

                                add_string_after_specific_string(active_code_list->end, "LABEL $if_end_");
                                active_code_list->end->is_start_of_new_line = true;
                                sprintf(str, "%d", if_end_num);
                                append_text_to_specific_string(active_code_list->end, str);

                                if(im_parent_while_or_if)
                                    im_in_while_or_if = false;


                                return true;
                            }
                        }
                    }
                }
            }
        }
    } else if(token.type == WHILE){        // 26. State -> while Expr do eol St_list end eol

        // For nested while loops or if statement
        bool im_parent_while_or_if = false;

        char str[12];       // 12 because 32bit int cant have higher value
        int condition_num = generic_label_count++;

        // Place DEFVAR at good place, if we are in while loop or if statement, above that label
        Tstring good_place_for_defvar = active_code_list->end;
        if(im_in_while_or_if){
            good_place_for_defvar = find_nearest_good_place_for_defvar()->prev;
        }

        add_string_after_specific_string(good_place_for_defvar, "DEFVAR LF@condition_");
        good_place_for_defvar->next->is_start_of_new_line = true;
        sprintf(str, "%d", condition_num);
        append_text_to_specific_string(good_place_for_defvar->next, str);

        add_string_after_specific_string(active_code_list->end, "LABEL $while_");
        active_code_list->end->is_start_of_new_line = true;
        int while_num = generic_label_count++;
        sprintf(str, "%d", while_num);
        append_text_to_specific_string(active_code_list->end, str);

        if(!im_in_while_or_if){
            active_code_list->end->before_me_is_good_place_for_defvar = true;
            im_parent_while_or_if = true;
        }

        im_in_while_or_if = true;

        pop();
        if(!expr())
            return false;

        add_string_after_specific_string(active_code_list->end, "POPS LF@condition_");
        active_code_list->end->is_start_of_new_line = true;
        sprintf(str, "%d", condition_num);
        append_text_to_specific_string(active_code_list->end, str);

        add_string_after_specific_string(active_code_list->end, "JUMPIFNEQ $while_end_");
        active_code_list->end->is_start_of_new_line = true;
        int while_end_num = generic_label_count++;
        sprintf(str, "%d", while_end_num );
        append_text_to_specific_string(active_code_list->end, str);
        add_string_after_specific_string(active_code_list->end, "LF@condition_");
        sprintf(str, "%d", condition_num);
        append_text_to_specific_string(active_code_list->end, str);
        add_string_after_specific_string(active_code_list->end, "bool@true");

        if(token.type == DO ) {
            pop();
            if(token.type == EOL_CASE ) {
                pop();
                if(!st_list())
                    return false;
                if(token.type == END ) {
                    pop();
                    if(token.type == EOL_CASE ) {
                        pop();

                        add_string_after_specific_string(active_code_list->end, "JUMP $while_");
                        active_code_list->end->is_start_of_new_line = true;
                        sprintf(str, "%d", while_num);
                        append_text_to_specific_string(active_code_list->end, str);

                        add_string_after_specific_string(active_code_list->end, "LABEL $while_end_");
                        active_code_list->end->is_start_of_new_line = true;
                        sprintf(str, "%d", while_end_num);
                        append_text_to_specific_string(active_code_list->end, str);

                        // Because while always return nil
                        add_string_after_specific_string(active_code_list->end, "PUSHS nil@nil");
                        active_code_list->end->is_start_of_new_line = true;

                        if(im_parent_while_or_if)
                            im_in_while_or_if = false;

                        return true;
                    }
                }
            }
        }
    }

    return false;
}

bool main_body(){

    // 2. Main_body -> State Main_body
    if(token.type == IDENTIFICATOR || token.type == EXPR || token.type == IF ||
       token.type == WHILE ){
        return state() && main_body();
    }else if(token.type == DEF){               // 3. Main_body -> Def_func Main_body
        return def_func() && main_body();
    } else if(token.type == EOF_CASE){            // 4. Main_body -> eps
        pop();

        return true;
    }

    return false;           // If no rule applied
}

bool prog(){

    // rule 1. Prog -> Main_body
    if(token.type == IDENTIFICATOR || token.type == EXPR || token.type == IF ||
       token.type == WHILE || token.type == DEF || token.type == EOF_CASE){
        return main_body();
    }

    return false;       // If no rule applied
}

void init_parser(){
    // For throwing away EOL if is as start of file
    im_in_while_or_if = false;
    token.type = EOL_CASE;
    code_list_init();
    generic_label_count = 0;

    global_symtable_init(&global_symtable);
    actual_symtable = global_symtable;
    add_string_after_specific_string(functions_code_list, ".IFJcode18");

    functions_code_list = active_code_list;

    // Useless, but you get the point, function_code_list is now "active"
    active_code_list = functions_code_list;

    add_string_after_specific_string(active_code_list->end, "JUMP $$main");
    active_code_list->end->is_start_of_new_line = true;

    generate_system_functions();

    active_code_list = main_code_list;
    add_string_after_specific_string(active_code_list, "LABEL $$main");
    active_code_list->is_start_of_new_line = true;
    add_string_after_specific_string(active_code_list->end, "CREATEFRAME");
    active_code_list->end->is_start_of_new_line = true;
    add_string_after_specific_string(active_code_list->end, "PUSHFRAME");
    active_code_list->end->is_start_of_new_line = true;

}

int main(){

    init_parser();

    pop();      // get first token

    // Start analyser
    bool analysis_result = prog();

    if(!analysis_result && error_code == 0){
        error_code = 2;
    }

    if(error_code != 0){
        // Free stuff if error
        free_symtable(&global_symtable);
        free_code_lists();
    }

    // Error handling
    switch(error_code){
        case 0:
            // Print functions definitions
            active_code_list = functions_code_list;
            print_code();

            // Print main body
            active_code_list = main_code_list;
            print_code();

            // Free stuff
            free_symtable(&global_symtable);
            free_code_lists();
            return 0;
        case 1:
            fprintf(stderr,"\nLex error!\n\n");
            return error_code;
        case 2:
            fprintf(stderr,"\nAnalysis error!\n\n");
            return error_code;
        case 3:
            fprintf(stderr,"\nNon define variable/function or redefinition attempt error!\n\n");
            return error_code;
        case 4:
            fprintf(stderr,"\nWrong compatibility types in expression error!\n\n");
            return error_code;
        case 5:
            fprintf(stderr,"\nWrong number of parameters when calling function error!\n\n");
            return error_code;
        case 6:
            fprintf(stderr,"\nOther sematic error!\n\n");
            return error_code;
        case 9:
            fprintf(stderr,"\nDivide by zero error!\n\n");
            return error_code;
        case 99:
            fprintf(stderr,"\nInternal program error!\n\n");
            return error_code;
        default:
            return error_code;
    }
}

