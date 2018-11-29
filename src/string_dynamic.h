//
// Created by Dominik Vagala on 29. 11. 2018.
//

#ifndef IFJ_PROJECT_STRING_DYNAMIC_H
#define IFJ_PROJECT_STRING_DYNAMIC_H




typedef struct{
    char *text;
    int allocated_space;
    int free_space;
}Tstring;

typedef struct Sstring_pile_list{
    Tstring string;

    struct Sstring_pile_list *prev;
    struct Sstring_pile_list *next;
    struct Sstring_pile_list *start;
    struct Sstring_pile_list *end;
}*Tstring_pile_list;

void create_string( Tstring *string, char *text);

void print_pile_of_strings();

void init_string(Tstring_pile_list *string_pile_list, Tstring *string);

void add_string_to_pile_list(Tstring *string);

void append_to_string(Tstring *string ,char *text);

void free_pile_of_strings();

//void append_string(Tstring_dynamic *base, char *append);

#endif //IFJ_PROJECT_STRING_DYNAMIC_H
