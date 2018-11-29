//
// Created by Dominik Vagala on 29. 11. 2018.
//

#ifndef IFJ_PROJECT_STRING_DYNAMIC_H
#define IFJ_PROJECT_STRING_DYNAMIC_H

/**List of strings for code_generator, where I can do append, but only on last string on list
 * Purpose of this list is mainly because, I need somehow store all strings,
 * so I can free them later, when I generate whole code
 * */
typedef struct Sstring{
    char *text;

    struct Sstring *prev;
    struct Sstring *next;
    struct Sstring *start;
    struct Sstring *end;
}*Tstring;

Tstring string_list;

void print_list_of_strings();

char *append_to_string(char *text);

void print_list_of_strings_back();

void add_string_to_list(char* text);

void free_list_of_strings();


#endif //IFJ_PROJECT_STRING_DYNAMIC_H
