#include <stdio.h>
#include <malloc.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include "scanner.h"


static char* Keywords[KEYWORD_COUNT]={"IF","ELSE","DEF","DO","END","NOT","NIL","THEN","WHILE"};
int main() {

    char token[50];
    int c;

    c=getchar();
    Tokens_Types state=0;


    if (isalpha(c))
    {

        state = 50;
    }

    if (isdigit(c))
    {
        state= 51;
    }

    if(c == '+')
    {
        state = 52;
    }
    if(c == '-')
    {
        state = 53;
    }
    if(c == '*')
    {
        state = MULT;
    }
    if(c == '%')
    {
        state = MODULO;
    }
    if(c == '/')
    {
        state = DIVIDE;
    }
    if(c == '=')
    {
        state = 54;
    }
    if(c == '{')
    {
        state = LBRA;
    }
    if(c == '}')
    {
        state = RBRA;
    }
    if(c == ';')
    {
        state = SEMICOLON;
    }
    if(c == '(')
    {
        state = LPAR;
    }
    if(c == ')')
    {
        state = RPAR;
    }

    switch(state)
    {
        int i = 0;
        case 50://IDENTIFICATOR OR A KEY WORD
        {
            printf("I am here");
            while (isalpha(c) || c=='_')
            {
                token[i] = c;
                c = getchar();
                i++;
                for (int i=0;i<KEYWORD_COUNT;i++)
                {
                    if(strcmp(token,Keywords[i])==0)
                    {
                        if (c=getchar()==' ')
                        {
                            case 100: //100 is word
                            {
                                if(strcmp(token, "IF")
                                {
                                   case IF:
                                }
                                else if(strcmp(token, "ELSE")
                                {
                                    case ELSE:
                                }
                                else if(strcmp(token, "DEF")
                                {
                                    case DEF:
                                }
                                else if(strcmp(token, "DO")
                                {
                                    case DO:
                                }
                                else if(strcmp(token, "END")
                                {
                                    case END:
                                }
                                else if(strcmp(token, "NIL")
                                {
                                    case NIL:
                                }
                                else if(strcmp(token, "THEN")
                                {
                                    case THEN:
                                }
                                else if(strcmp(token, "WHILE")
                                {
                                    case WHILE:
                                }
                                else if(strcmp(token, "NOT")
                                {
                                    case NOT:
                                }

                            }
                        }
                    }
                }
            }

            for(int i=0;token[i]!='\0';i++)
            {
                printf("%c \n",token[i]);
            }
        }

        case 51:
        {
            while (isdigit(c))
            {
                token[i] = c;
                c = getchar();
                i++;
            }
            printf("%s", token);
        }

        case 52:
        {
            token[i]=c;
            c = getchar();
            if (c=='+')
            {
                case INC:
                {

                }
            }
            else
                case PLUS:
                {

                }
        }

        case 53:
        {
            if (c=='-')
            {
                case DEC:
                {

                }
            }
            else
                case MINUS:
                {

                }
        }

        case LPAR:
        {

        }

        case IF:break;
        case ELSE:break;
        case DEF:break;
        case DO:break;
        case END:break;
        case NIL:break;
        case THEN:break;
        case WHILE:break;
        case INT:break;
        case STRING:break;
        case MULT:break;
        case MODULO:break;
        case DIVIDE:break;
        case ASSIGN:break;
        case LESS:break;
        case MORE:break;
        case LOE:break;
        case MOE:break;
        case EQUAL:break;
        case LBRA:break;
        case RBRA:break;
        case SEMICOLON:break;
        case RPAR:break;
        case nil:break;
        case ID:break;
        case SINGLECOM:break;
        case BEGCOM:break;
        case ENDCOM:break;
    }





    return 0;
}
