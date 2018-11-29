//
// Created by Dominik Vagala on 18. 11. 2018.
//

#include "parser.h"
#include "scanner.h"
#include "code_gen.h"
#include "string_dynamic.h"
#include "expressions.h"

int error_code = 0;

tToken token;
tToken previousToken;       // always behind actual token
tToken aheadToken;          // When I need to do lookahead what is after actual token, this is it
bool tokenLookAheadFlag = false;

// If we are in definition of function this is local_symtable of that function, otherwise == global_symtable
Bnode actual_symtable;
Bnode global_symtable;

Tcode_list code_list;
Tstring_pile_list string_pile_list;

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

    if(tokenLookAheadFlag){
        printf("Can do look ahead for only one token!\n");
        exit(1);
    }

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

    printf("Checking if is_token_start_of_expr\n");
//    printf("---------------------------------------------------------\n");

    if (token.type == IDENTIFICATOR) {
        if(is_id_variable(&actual_symtable, token.data.string)){
            aheadToken = next_token_lookahead();
            if(aheadToken.type != ASSIGN && previousToken.type != LPAR &&
                previousToken.type != COLON && previousToken.type != IDENTIFICATOR){
                // There are only 3 cases when id_variable is not start of expression
                // 1. id_variable = restOfCode
                // 2. def id_func(id_variable, id_variable)
                // 3. def id_func id_variable, id_variable
                printf("--Start of expr: First\n");
                return true;
            }
        }
    } else if(token.type == INT || token.type == STRING || token.type == FLOAT || token.type == FLOAT_EXPO){
        // In this cases it is not start of expression
        // 1. id_func 45, "hoho"
        // 2. id_func (45, "hoho")
        if(previousToken.type != IDENTIFICATOR && previousToken.type != COLON && previousToken.type != LPAR){
            printf("--Start of expr: Second\n");
            return true;
        }
    } else if(token.type == LPAR && previousToken.type != IDENTIFICATOR){
        // There are only two cases when '(' is not start of expression
        // 1. id = id(id, id)
        // 2. def id(id, id)
        // So if token before '(' was id it is certainly not an expression
        printf("--Start of expr: Third\n");
        return true;
    }

    return false;
}

/**Pop token out of the 'stack'*/
void pop(){

    printf("Poping\n");
    printf("Old token type: %s\n", token_type_enum_string[token.type]);
    token = enhanced_next_token();
    printf("New token type: %s\n", token_type_enum_string[token.type]);

    if(token.type == ERROR && error_code == 0){          // Check lex error
        error_code = 1;
        return;
    }

    if(is_token_start_of_expr()){
        original_token_type_backup = token.type;      // Store original token type, so later i can be reverted and handed to expression analyzer
        token.type = EXPR;                            // override token type, so we can handle it easily with rules
        printf("End of isTokenStartOfExpr\n");
    }
}

///**Fake expression analyzer, just read epression and came back when expression ends*/
//tToken analyze_expression(tToken token, tToken aheadToken, bool lookahead_occured){
//
//    printf("Analyzing expression start token: %s\n", token_type_enum_string[token.type]);
//    printf("Analyzing expression lookahead token: %s\n", token_type_enum_string[aheadToken.type]);
//
//
//    tToken temp = enhanced_next_token();
//
//    printf("iterating... 0\n");
//
//
//    while(temp.type != EOL_CASE && temp.type != THEN && temp.type != DO){
//        temp = nextToken();
//        printf("iterating... 0\n");
//    }
//
//    tokenLookAheadFlag = false;
//
//    printf("Expression analyzer returning token: %s\n", token_type_enum_string[temp.type]);
//
//    return temp;
//}


/**----------RULES------------------------------------*/


bool expr(){
    char non_term[] = "expr";
    printf("Im in %s\n", non_term);
    printf("token type: %s\n", token_type_enum_string[token.type]);

    // Set back original token type
    token.type = original_token_type_backup;

//    typedef struct {
//        tToken *token;
//        bool error;
//        int error_code;
//    }ReturnData;
//
    ReturnData returnData;
    returnData = analyze_expresssion(token, aheadToken, tokenLookAheadFlag, &global_symtable);

    token = (*returnData.token);

    if(error_code == 0 && returnData.error){
        printf("Error from expression\n");
        error_code = returnData.error_code;
    }

    printf("after, epxresssoin\n");
    printf("token type: %s\n", token_type_enum_string[token.type]);

    // Fake expression analyzer, just read epression and came back when expression ends
//    token = analyze_expression(token, aheadToken, tokenLookAheadFlag);

    printf("%s returning: %d\n", non_term, 1);
    return true;
}

bool more_param(char *func_name){
    char non_term[] = "more_param";
    printf("Im in %s\n", non_term);
    printf("token type: %s\n", token_type_enum_string[token.type]);

    // 33. More_params -> eps
    if(token.type == RPAR){
        printf("%s returning: %d\n", non_term, 1);
        return true;
    } else if(token.type == COLON){        // 32. More_params -> , id More_params
        pop();
        if(token.type == IDENTIFICATOR){

            // Semantics. Add var_name to func_name parameters, if var_name was already there error 3
            char *var_name = token.data.string;

            if(is_variable_already_in_func_params(&global_symtable, func_name, var_name)){
                printf("-Semantics: %s, variable already in function params\n", var_name);
                // This ensures that only first error will be in error_code
                printf("1\n");
                if(error_code == 0)
                    error_code = 3;
            }

            if(!add_variable_to_func_params(&global_symtable, func_name, var_name)){
                // This ensures that only first error will be in error_code
                if(error_code == 0)
                    error_code = 99;
            }

            printf("-Semantics: adding param: %s to func: %s\n", var_name, func_name);

            if(!add_variable_to_symtable(&actual_symtable, var_name)){
                // This ensures that only first error will be in error_code
                if(error_code == 0)
                    error_code = 99;
            }
            printf("-Semantics: adding param: %s from func: %s, to its local symtable as variable\n", var_name, func_name);

            pop();
            return more_param(func_name);
        }
    }

    printf("%s returning: %d\n", non_term, 0);
    return false;
}

bool param(char *func_name){
    char non_term[] = "param";
    printf("Im in %s\n", non_term);
    printf("token type: %s\n", token_type_enum_string[token.type]);

    // 31. Param -> id More_params
    if(token.type == IDENTIFICATOR){

        // Semantics. Add var_name to func_name parameters, if var_name was already there error 3
        char *var_name = token.data.string;

        if(is_variable_already_in_func_params(&global_symtable, func_name, var_name)){
            // This ensures that only first error will be in error_code
            printf("-Semantics: %s, variable already in function params\n", var_name);
            if(error_code == 0)
                error_code = 3;
        }

        // return false when malloc fails
        if(!add_variable_to_func_params(&global_symtable, func_name, var_name)){
            // This ensures that only first error will be in error_code
            if(error_code == 0)
                error_code = 99;
        }
        printf("-Semantics: adding param: %s to func: %s\n", var_name, func_name);


        if(!add_variable_to_symtable(&actual_symtable, var_name)){
            // This ensures that only first error will be in error_code
            if(error_code == 0)
                error_code = 99;
        }
        printf("-Semantics: adding param: %s from func: %s, to its local symtable as variable\n", var_name, func_name);

        pop();
        return more_param(func_name);
    } else if(token.type == RPAR){        // 30. Param -> eps
        printf("%s returning: %d\n", non_term, 1);
        return true;
    }

    printf("%s returning: %d\n", non_term, 0);
    return false;
}

bool def_func(){
    char non_term[] = "def_func";
    printf("Im in %s\n", non_term);
    printf("token type: %s\n", token_type_enum_string[token.type]);

    // 27. Def_func -> def id ( Param ) eol St_list end eol
    if(token.type == DEF) {
        pop();
        if (token.type == IDENTIFICATOR) {

            char *func_name = token.data.string;

            // Semantics. Check redefinition of func, if yes -> error 3
            // Semantics. Check if func_name isnt previously declared global variable, if yes -> error 3
            // Semantics. Add id_func to symtable, return false if malloc fail

            if(is_func_defined(&global_symtable, func_name)){
                // This ensures that only first error will be in error_code
                printf("-Semantics: %s, calling funciton wanst declared\n", func_name);
                if(error_code == 0)
                    error_code = 3;
            }

            if(has_func_same_name_as_global_variable(&global_symtable, func_name)){
                printf("-Semantics: %s, funciton has same name as global variable\n", func_name);
                // This ensures that only first error will be in error_code
                if(error_code == 0)
                    error_code = 3;
            }

            if(!add_func_to_symtable(&global_symtable, func_name)){
                // This ensures that only first error will be in error_code
                if(error_code == 0)
                    error_code = 99;
            }

            printf("-Semantics: adding func: %s to symtable\n", func_name);

            // New local symtable for this function
            local_symtable_init(&actual_symtable);
            printf("-Semantics: switching context to local\n");

            pop();
            if (token.type == LPAR) {
                pop();
                if (!param(func_name))
                    return false;
                if (token.type == RPAR) {
                    pop();
                    if (token.type == EOL_CASE) {
                        pop();
                        if (!st_list())
                            return false;
                        if (token.type == END) {
                            free_symtable(&actual_symtable);
                            actual_symtable = global_symtable;              // switch context back to global
                            printf("-Semantics: switching context to global\n");
                            pop();
                            if (token.type == EOL_CASE) {
                                pop();
                                printf("%s returning: %d\n", non_term, 1);
                                return true;
                            }
                        }
                    }
                }
            }
        }
    }

    printf("%s returning: %d\n", non_term, 0);
    return false;
}

bool more_args(int *num_of_args){
    char non_term[] = "more_args";
    printf("Im in %s\n", non_term);
    printf("token type: %s\n", token_type_enum_string[token.type]);

    // 17. More-args -> eps
    if(token.type == EOL_CASE || token.type == RPAR){
        printf("%s returning: %d\n", non_term, 1);
        return true;
    } else if(token.type == COLON){         // 16. More-args -> , Term More-args
        pop();
        (*num_of_args)++;                   // So we found another argument
        return term() && more_args(num_of_args);
    }

    printf("%s returning: %d\n", non_term, 0);
    return false;
}

bool term(){
    char non_term[] = "term";
    printf("Im in %s\n", non_term);
    printf("token type: %s\n", token_type_enum_string[token.type]);

    // 18. Term -> id
    if(token.type == IDENTIFICATOR){

        char *var_name = token.data.string;

        // Semantics. Check if var_name was defined as variable, if not error 3
        if(!is_variable_defined(&actual_symtable, var_name)){
            printf("-Semantics: %s, variable wanst declared\n", var_name);
            // This ensures that only first error will be in error_code
            if(error_code == 0)
                error_code = 3;
        }

        pop();
        printf("%s returning: %d\n", non_term, 1);
        return true;
    } else if(token.type == INT){           // 19. Term -> int
        pop();
        printf("%s returning: %d\n", non_term, 1);
        return true;
    } else if(token.type == FLOAT){         // 20. Term -> double
        pop();
        printf("%s returning: %d\n", non_term, 1);
        return true;
    } else if(token.type == STRING){        // 21. Term -> string
        pop();
        printf("%s returning: %d\n", non_term, 1);
        return true;
    } else if(token.type == NIL){           // 22. Term -> nil
        pop();
        printf("%s returning: %d\n", non_term, 1);
        return true;
    }

    printf("%s returning: %d\n", non_term, 0);
    return false;
}

bool arg_in_brackets(int *num_of_args){
    char non_term[] = "arg_in_brackets";
    printf("Im in %s\n", non_term);
    printf("token type: %s\n", token_type_enum_string[token.type]);

    // 15. Arg_in_brackets -> Term More-args
    if(token.type == IDENTIFICATOR || token.type == INT || token.type == FLOAT ||
       token.type == STRING || token.type == NIL){

        // '1' becase if we get here, there will be ceratily at least one term + more_arg
        *num_of_args = 1;

        return term() && more_args(num_of_args);
    } else if(token.type == RPAR){            // 14. Arg_in_brackets -> eps
        printf("%s returning: %d\n", non_term, 1);
        return true;
    }

    printf("%s returning: %d\n", non_term, 0);
    return false;
}

bool call_func_args(int *num_of_args){
    char non_term[] = "call_func_args";
    printf("Im in %s\n", non_term);
    printf("token type: %s\n", token_type_enum_string[token.type]);

    // 13. Call_func_args -> Term More-args
    if(token.type == IDENTIFICATOR || token.type == INT || token.type == FLOAT ||
       token.type == STRING || token.type == NIL){

        // '1' becase if we get here, there will be ceratily at least one term + more_arg
        *num_of_args = 1;

        return term() && more_args(num_of_args);
    } else if(token.type == EOL_CASE){            // 11. Call_func_args -> eps
        return true;
    } else if(token.type == LPAR){            // 12. Call_func_args -> ( Arg_in_brackets )
        pop();

        if(!arg_in_brackets(num_of_args))
            return false;
        if(token.type == RPAR){
            pop();
            return true;
        }
    }

    printf("%s returning: %d\n", non_term, 0);
    return false;
}

bool call_func(){
    char non_term[] = "call_func";
    printf("Im in %s\n", non_term);
    printf("token type: %s\n", token_type_enum_string[token.type]);

    // 8. Call_func -> id Call_func_args
    if(token.type == IDENTIFICATOR){

        char *func_name = token.data.string;
        pop();

        // Semantics. Check if func_name was defined before calling, if not error 3
        if(!is_func_defined(&global_symtable, func_name)){
            printf("-Semantics: %s, calling funciton wanst declared\n", func_name);
            // This ensures that only first error will be in error_code
            if(error_code == 0)
                error_code = 3;
        }

        int num_of_args = 0;
        // num_of_args is handed by pointer, so sub_functions can later increment its value if there is another argument
        bool sub_analysis_result = call_func_args(&num_of_args);

        // Semantics. Check if num_of_args == func_name.defined.parameters, if not error 5
        // If number of define_params == -1,
        // it means number of parameters is variable (for system func print()), thus skip error scope
        int defined_params = get_num_of_defined_func_params(&global_symtable, func_name);
        if(num_of_args != defined_params && defined_params != -1){
            printf("-Semantics: %s: number of arg not quals params\n", func_name);
            // This ensures that only first error will be in error_code
            if(error_code == 0)
                error_code = 5;
        }

        printf("-Semantics: Function: %s is calling with: %d arguments.\n", func_name, num_of_args);

        return sub_analysis_result;
    }

    printf("%s returning: %d\n", non_term, 0);
    return false;
}

bool func_or_expr(){
    char non_term[] = "func_or_expr";
    printf("Im in %s\n", non_term);
    printf("token type: %s\n", token_type_enum_string[token.type]);

    // 7. Func_or_expr -> Call_func
    if(token.type == IDENTIFICATOR){
        return call_func();
    } else if(token.type == EXPR){      // 9. Func_or_expr -> Expr
        return expr();
    }

    printf("%s returning: %d\n", non_term, 0);
    return false;
}

bool after_id() {
    char non_term[] = "after_id";
    printf("Im in %s\n", non_term);
    printf("token type: %s\n", token_type_enum_string[token.type]);

    // 10. After_id -> Call_func_args
    if(token.type == IDENTIFICATOR || token.type == EOL_CASE || token.type == LPAR ||
       token.type == INT || token.type == FLOAT || token.type == STRING || token.type == NIL){

        // previous_token == id_func      // cause we are in after_id
        char *func_name = previousToken.data.string;

        // Semantics. Check if func_name was defined before calling, if not error 3
        if(!is_func_defined(&global_symtable, func_name)){
            printf("-Semantics: %s, calling funciton wanst declared\n", func_name);
            // This ensures that only first error will be in error_code
            if(error_code == 0)
                error_code = 3;
        }

        int num_of_args = 0;
        // num_of_args is handed by pointer, so sub_functions can later increment its value if there is another argument
        bool sub_analysis_result = call_func_args(&num_of_args);

        // Semantics. Check if num_of_args == func_name.defined.parameters, if not error 5
        // If number of define_params == -1,
        // it means number of parameters is variable (for system func print()), thus skip error scope
        int defined_params = get_num_of_defined_func_params(&global_symtable, func_name);
        if(num_of_args != defined_params && defined_params != -1){
            printf("-Semantics: %s: number of arg not quals params\n", func_name);
            // This ensures that only first error will be in error_code
            if(error_code == 0)
                error_code = 5;
        }

        printf("-Semantics: Function: %s is calling with: %d arguments.\n", func_name, num_of_args);

        return sub_analysis_result;
    }else if(token.type == ASSIGN){        // 6. After_id -> = Func_or_expr

        // previous_token == id_var      // cause we are in after_id
        char *var_name = previousToken.data.string;

        printf("var_name: %s\n", var_name);
        printf("is var_name defined: %d\n", is_variable_defined(&global_symtable, var_name));
        printf("Is var_name in funcions: %d\n", is_func_defined(&global_symtable, var_name));

        // Semantics. Check if var_name isnt previously declared function -> error 3
        // Semantics. Add var_name to symtable if not already
        if(has_variable_same_name_as_func(&global_symtable, var_name)){
            printf("-Semantics: %s: variable has same name as function\n", var_name);
            // This ensures that only first error will be in error_code
            if(error_code == 0)
                error_code = 3;
        }

        // TODO: Generate code for define variables
        if(!is_variable_defined(&actual_symtable, var_name)){
            if(actual_symtable == global_symtable){
            }
        }

        // return false when malloc fails
        if(!add_variable_to_symtable(&actual_symtable, var_name)){
            // This ensures that only first error will be in error_code
            if(error_code == 0)
                error_code = 99;
        }

        printf("-Semantics: adding variable: %s to symtable\n", var_name);

        pop();
        return func_or_expr();
    }

    printf("%s returning: %d\n", non_term, 0);
    return false;
}

bool st_list() {
    char non_term[] = "st_list";
    printf("Im in %s\n", non_term);
    printf("token type: %s\n", token_type_enum_string[token.type]);

    // 28. St_list -> State St_list
    if (token.type == IDENTIFICATOR || token.type == EXPR || token.type == IF || token.type == WHILE) {
        return state() && st_list();
    } else if (token.type == ELSE || token.type == END) {      // 29. St_list -> eps
        return true;
    }

    printf("%s returning: %d\n", non_term, 0);
    return false;
}

bool state(){
    char non_term[] = "state";
    printf("Im in %s\n", non_term);
    printf("token type: %s\n", token_type_enum_string[token.type]);

    // 5. State -> id After_id eol
    if(token.type == IDENTIFICATOR ){
        pop();
        if(!after_id())
            return false;
        if(token.type == EOL_CASE ) {
            pop();
            printf("%s returning: %d\n", non_term, 1);
            return true;
        }
    } else if(token.type == EXPR){      // 24. State -> Expr eol
        if(!expr())
            return false;
        if(token.type == EOL_CASE ) {
            pop();
            printf("%s returning: %d\n", non_term, 1);
            return true;
        }
    } else if(token.type == IF){        // 25. State -> if Expr then eol St_list else eol St_list end eol
        pop();
        if(!expr())
            return false;
        if(token.type == THEN ) {
            pop();
            if(token.type == EOL_CASE ) {
                pop();
                if(!st_list())
                    return false;
                if(token.type == ELSE ) {
                    pop();
                    if(token.type == EOL_CASE ) {
                        pop();
                        if(!st_list())
                            return false;
                        if(token.type == END ) {
                            pop();
                            if(token.type == EOL_CASE ) {
                                pop();
                                printf("%s returning: %d\n", non_term, 1);
                                return true;
                            }
                        }
                    }
                }
            }
        }
    } else if(token.type == WHILE){        // 26. State -> while Expr do eol St_list end eol
        pop();
        if(!expr())
            return false;
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
                        printf("%s returning: %d\n", non_term,  1);
                        return true;
                    }
                }
            }
        }
    }

    printf("%s returning: %d\n", non_term, 0);
    return false;
}

bool main_body(){
    char non_term[] = "main_body";
    printf("Im in %s\n", non_term);
    printf("token type: %s\n", token_type_enum_string[token.type]);

    // 2. Main_body -> State Main_body
    if(token.type == IDENTIFICATOR || token.type == EXPR || token.type == IF ||
       token.type == WHILE ){
        return state() && main_body();
    }else if(token.type == DEF){               // 3. Main_body -> Def_func Main_body
        return def_func() && main_body();
    } else if(token.type == EOF_CASE){            // 4. Main_body -> eps
        pop();
        printf("%s returning: %d\n", non_term,  1);
        return true;
    }

    printf("%s returning: %d\n", non_term, 0);
    return false;           // If no rule applied
}

bool prog(){
    char non_term[] = "prog";
    printf("Im in %s\n", non_term);
    printf("token type: %s\n", token_type_enum_string[token.type]);

    // rule 1. Prog -> Main_body
    if(token.type == IDENTIFICATOR || token.type == EXPR || token.type == IF ||
       token.type == WHILE || token.type == DEF || token.type == EOF_CASE){
        return main_body();
    }

    printf("%s returning: %d\n", non_term, 0);
    return false;       // If no rule applied
}

/** Just test if scanner works properly
 *  Print tokens as types, not numbers
 * */
void test_scanner(){
    printf("\nTest scanner:\n\n");

    while(token.type != EOF_CASE){
//        token = enhanced_next_token();
        token = enhanced_next_token();
        printf("%s ", token_type_enum_string[token.type]);
//        if(token.type == IDENTIFICATOR)
//            printf("(token.data.string = \"%s\") ", token.data.string);
        if(token.type == EOL_CASE)
            printf("\n");
    }
    printf("\n");
    printf("\n");
}

void test_symtable(){

    global_symtable_init(&global_symtable);

    printf("non define: %d\n", is_variable_defined(&global_symtable, "id_var1"));

    add_variable_to_symtable(&global_symtable, "id_var1");
    add_variable_to_symtable(&global_symtable, "id_var2");
    add_variable_to_symtable(&global_symtable, "id_var3");

    printf("is define: %d\n", is_variable_defined(&global_symtable, "id_var1"));
    printf("is define: %d\n", is_variable_defined(&global_symtable, "id_var3"));
    printf("noon define: %d\n", is_variable_defined(&global_symtable, "id_var4"));

    printf("non define: %d\n", is_variable_defined(&global_symtable, "id_func1"));

    add_func_to_symtable(&global_symtable, "id_func1");
    add_func_to_symtable(&global_symtable, "id_func2");
    add_func_to_symtable(&global_symtable, "id_func3");

    printf("is define: %d\n", is_variable_defined(&global_symtable, "id_func1"));
    printf("is define: %d\n", is_variable_defined(&global_symtable, "id_func3"));
    printf("noon define: %d\n", is_variable_defined(&global_symtable, "id_func4"));

    printf("is id variable: %d\n", is_id_variable(&global_symtable, "id_var3"));
    printf("not id variable: %d\n", is_id_variable(&global_symtable, "id_var5"));
    printf("not id variable: %d\n", is_id_variable(&global_symtable, "id_func3"));

    add_variable_to_func_params(&global_symtable, "id_func1", "id_param1");
    add_variable_to_func_params(&global_symtable, "id_func1", "id_param2");
    add_variable_to_func_params(&global_symtable, "id_func2", "id_param3");

    printf("is variable in func params: %d\n", is_variable_already_in_func_params(&global_symtable, "id_func1","id_param1"));
    printf("is variable in func params: %d\n", is_variable_already_in_func_params(&global_symtable, "id_func1","id_param2"));
    printf("is variable in func params: %d\n", is_variable_already_in_func_params(&global_symtable, "id_func2","id_param3"));
    printf("is not variable in func params: %d\n", is_variable_already_in_func_params(&global_symtable, "id_func2","id_param4"));

    printf("func has same name: %d\n",has_func_same_name_as_global_variable(&global_symtable, "id_var1"));
    printf("func has same name: %d\n",has_func_same_name_as_global_variable(&global_symtable, "id_var3"));
    printf("func has not same name: %d\n",has_func_same_name_as_global_variable(&global_symtable, "id_var5"));

    printf(" var has same name: %d\n",has_variable_same_name_as_func(&global_symtable, "id_func1"));
    printf(" var has same name: %d\n",has_variable_same_name_as_func(&global_symtable, "id_func3"));
    printf(" var has not same name: %d\n",has_variable_same_name_as_func(&global_symtable, "id_func5"));

    printf("Advanced\n");
    add_variable_to_func_params(&global_symtable, "id_func6", "id_param3");
    printf("is not variable in func params: %d\n", is_variable_already_in_func_params(&global_symtable, "id_func7","id_param1"));
}

void test_code_list(){

    init_code_list(&code_list);
//
//    add_code_line(&code_list, "DEFVAR", "GF@counter", NULL, NULL);
//    add_code_line(&code_list, "MOVE", "GF@counter", "string@", NULL);
//    add_code_line(&code_list, "LABEL", "GF@counter", "string@", NULL);

}

void test_strings(){

    Tstring string;

    create_string(&string, "first_");
    append_to_string(&string, "hohoo");

    printf("poin text: %p\n", (void*) string.text);

    add_string_to_pile_list( &string);

    Tstring string2;
    string2.text = NULL;

    create_string(&string2, "hohoo2");
    append_to_string(&string2, "_append");

    printf("poin text: %p\n", (void*) string2.text);

    add_string_to_pile_list( &string2);

    Tstring string3;
    string3.text = NULL;

    printf("..pointer to string:%p\n", string3);
    printf("..pointer to tesxt:%p\n", string3.text);

    create_string(&string3, "hohoo3");
    printf("..pointer to string:%p\n", string3);
    printf("..pointer to tesxt:%p\n", string3.text);
    append_to_string(&string3, "_");
    printf("..pointer to string:%p\n", string3);
    printf("..pointer to tesxt:%p\n", string3.text);

    append_to_string(&string3, "a");
    printf("..pointer to string:%p\n", string3);
    printf("..pointer to tesxt:%p\n", string3.text);
    append_to_string(&string3, "bdddddddddddddddddddddddd");
    printf("..pointer to string:%p\n", string3);
    printf("..pointer to tesxt:%p\n", string3.text);

    printf("poin text: %p\n", (void*) string3.text);
    add_string_to_pile_list(&string3);


    print_pile_of_strings();

    free_pile_of_strings();
}

void test_expr(){

    pop();
//    analyze_expression(token, aheadToken, false );
    analyze_expresssion(token, aheadToken, false, &global_symtable);
}

int main(){

    // For throwing away EOL if is as start of file
    token.type = EOL_CASE;

//    test_scanner();
//    test_symtable();
//    test_code_list();
//    test_strings();
//        test_expr();
//    return 0;

    init_code_list(&code_list);

    global_symtable_init(&global_symtable);
    actual_symtable = global_symtable;

    pop();      // get first token

    // Start analyser
    bool analysis_result = prog();

    if(!analysis_result && error_code == 0){
        error_code = 2;
    }

    free_symtable(&global_symtable);

    // Error handling
    switch(error_code){
        case 1:
            printf("\nLex error!\n\n");
            return error_code;
        case 2:
            printf("\nAnalysis error!\n\n");
            return error_code;
        case 3:
            printf("\nNon define variable/function or redefinition attempt error!\n\n");
            return error_code;
        case 4:
            printf("\nWrong compatibility types in expression error!\n\n");
            return error_code;
        case 5:
            printf("\nWrong number of parameters when calling function error!\n\n");
            return error_code;
        case 6:
            printf("\nOther sematic error!\n\n");
            return error_code;
        case 9:
            printf("\nDivide by zero error!\n\n");
            return error_code;
        case 99:
            printf("\nInternal program error!\n\n");
            return error_code;
        default:
            printf("\nSuccess!\n\n");
            return error_code;
    }

    return 0;
}
