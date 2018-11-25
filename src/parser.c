//
// Created by Dominik Vagala on 18. 11. 2018.
//

#include "parser.h"

int error_code = 0;

tToken token;
tToken previousToken;       // always behind actual token
tToken aheadToken;          // When I need to do lookahead what is after actual token, this is it
bool tokenLookAheadFlag = false;


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
        return nextToken();
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

/** Another fake function, if id token has name "id_var" act as it is variable*/
bool isTokenVariable(){
    if(strcmp(token.data.string, "id_var") == 0)
        return true;
    return false;
}

/**
 *  id_var   =          id_var     +      5
 *           ^            ^        ^
 *    previous_token    token  aheadToken
 * */
bool is_token_start_of_expr() {

    if (token.type == IDENTIFICATOR) {
        if(isTokenVariable()){
            if(next_token_lookahead().type != ASSIGN && previousToken.type != LPAR && previousToken.type != COLON && previousToken.type != IDENTIFICATOR){
                // There are only 3 cases when id_variable is not start of expression
                // 1. id_variable = restOfCode
                // 2. def id_func(id_variable, id_variable)
                // 3. def id_func id_variable, id_variable
                printf("First\n");
                return true;
            }
        }
    } else if (token.type == INT || token.type == STRING ||
               token.type == FLOAT || token.type == FLOAT_EXPO){
        printf("Second\n");
        return true;
    } else if(token.type == LPAR && previousToken.type != IDENTIFICATOR){
        // There are only two cases when '(' is not start of expression
        // 1. id = id(id, id)
        // 2. def id(id, id)
        // So if token before '(' was id it is certainly not an expression
        printf("Third\n");
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

    if(is_token_start_of_expr()){
        token.type = EXPR;      // override token.type, so we can handle it easily with rules
        printf("Am in isTokenStartOfExpr\n");
    }
}

/**Fake expression analyzer, just read epression and came back when expression ends*/
tToken analyze_expression(tToken token, tToken aheadToken){

    tToken temp = nextToken();

    while(temp.type != EOL_CASE && temp.type != THEN && temp.type != DO){
        temp = nextToken();
    }

    return temp;
}



/**----------RULES------------------------------------*/


bool expr(){
    char non_term[] = "expr";
    printf("Im in %s\n", non_term);
    printf("token type: %s\n", token_type_enum_string[token.type]);

    // Fake expression analyzer, just read epression and came back when expression ends
    token = analyze_expression(token, aheadToken);

    printf("%s returning: %d\n", non_term, 1);
    return true;
}


bool more_param(){
    char non_term[] = "more_param";
    printf("Im in %s\n", non_term);
    printf("token type: %s\n", token_type_enum_string[token.type]);

    // 33. More_params -> eps
    if(token.type == RPAR){
        printf("%s returning: %d\n", non_term, 1);
        return true;
    } else if(token.type == COLON){        // 32. More_params -> , id More_params
        pop();
        return more_param();
    }

    printf("%s returning: %d\n", non_term, 0);
    return false;
}

bool param(){
    char non_term[] = "param";
    printf("Im in %s\n", non_term);
    printf("token type: %s\n", token_type_enum_string[token.type]);

    // 31. Param -> id More_params
    if(token.type == IDENTIFICATOR){
        pop();
        return more_param();
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

//            if(define_func(token) == 1) error;

            pop();
            if (token.type == LPAR) {
                pop();
                if (!param())
                    return false;
                if (token.type == RPAR) {
                    pop();
                    if (token.type == EOL_CASE) {
                        pop();
                        if (!st_list())
                            return false;
                        if (token.type == END) {
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

bool more_args(){
    char non_term[] = "more_args";
    printf("Im in %s\n", non_term);
    printf("token type: %s\n", token_type_enum_string[token.type]);

    // 17. More-args -> eps
    if(token.type == EOL_CASE || token.type == RPAR){
        printf("%s returning: %d\n", non_term, 1);
        return true;
    } else if(token.type == COLON){         // 16. More-args -> , Term More-args
        pop();
        return term() && more_args();
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

bool arg_in_brackets(){
    char non_term[] = "arg_in_brackets";
    printf("Im in %s\n", non_term);
    printf("token type: %s\n", token_type_enum_string[token.type]);

    // 15. Arg_in_brackets -> Term More-args
    if(token.type == IDENTIFICATOR || token.type == INT || token.type == FLOAT ||
       token.type == STRING || token.type == NIL){
        return term() && more_args();
    } else if(token.type == RPAR){            // 14. Arg_in_brackets -> eps
        printf("%s returning: %d\n", non_term, 1);
        return true;
    }

    printf("%s returning: %d\n", non_term, 0);
    return false;
}

bool call_func_args(){
    char non_term[] = "call_func_args";
    printf("Im in %s\n", non_term);
    printf("token type: %s\n", token_type_enum_string[token.type]);

    // 13. Call_func_args -> Term More-args
    if(token.type == IDENTIFICATOR || token.type == INT || token.type == FLOAT ||
       token.type == STRING || token.type == NIL){
        return term() && more_args();
    } else if(token.type == EOL_CASE){            // 11. Call_func_args -> eps
        return true;
    } else if(token.type == LPAR){            // 12. Call_func_args -> ( Arg_in_brackets )
        pop();
        if(!arg_in_brackets())
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
        pop();
        return call_func_args();
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
        return call_func_args();
    }else if(token.type == ASSIGN){        // 6. After_id -> = Func_or_expr
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
    } else if(token.type == ERROR)          // Check lex error
        error_code = 1;

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
    } else if(token.type == ERROR)          // Check lex error
        error_code = 1;

    printf("%s returning: %d\n", non_term, 0);
    return false;       // If no rule applied
}

/** Just test if scanner works properly
 *  Print tokens as types, not numbers
 * */
void test_scanner(){
    printf("\nTest scanner:\n\n");

    tToken token;
    while(token.type != EOF_CASE){
        token = nextToken();
        printf("%s ", token_type_enum_string[token.type]);
        if(token.type == EOL_CASE)
            printf("\n");
    }
    printf("\n");
    printf("\n");
}

int main(){

//    test_scanner();

    pop();      // get first token

    bool analysis_result = prog();

    //  free_resources(global tree, local tree)

    // Error handling
    if(error_code == 1){
        printf("\nLex error!\n\n");
        return 1;
    }
    else if(analysis_result == false){
        printf("\nAnalysis error!\n\n");
        return 2;
    }

    printf("\nAnalysis success! \n\n");

    return 0;

}