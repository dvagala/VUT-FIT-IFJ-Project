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
    if(c == '<')
    {
        state = 55;
    }
    if(c == '>')
    {
        state = 56;
    }
    if(c == '=')
    {
        state = 57;
    }
    switch(state)
    {
        int i = 0;
        case 50://IDENTIFICATOR OR A KEY WORD
        {
            printf("I am here");
            while (isalpha(c) || isdigit(c) || c=='_')
            {
                token[i] = c;
                c = getchar();
                i++;
                if (c=='?' || c=='!')
                {
                    token[i]=c;
                    case IDENTIFICATOR:
                    {

                    }
                }
                for (int i=0;i<KEYWORD_COUNT;i++)
                {
                    if(strcmp(token,Keywords[i])==0)
                    {
                        c=getchar();
                        if (c==' '||c=='\n')
                        {
                            case 100: //100 is word
                            {
                                if(strcmp(token, "IF") == 0)
                                {
                                   case IF:
                                }
                                else if(strcmp(token, "ELSE") == 0)
                                {
                                    case ELSE:
                                }
                                else if(strcmp(token, "DEF") == 0)
                                {
                                    case DEF:
                                }
                                else if(strcmp(token, "DO") == 0)
                                {
                                    case DO:
                                }
                                else if(strcmp(token, "END") == 0)
                                {
                                    case END:
                                }
                                else if(strcmp(token, "NIL") == 0)
                                {
                                    case NIL:
                                }
                                else if(strcmp(token, "THEN") == 0)
                                {
                                    case THEN:
                                }
                                else if(strcmp(token, "WHILE") == 0)
                                {
                                    case WHILE:
                                }
                                else if(strcmp(token, "NOT") == 0)
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
        //CASE FOR FLOAT AND INT
        case 51:
        {
            while (isdigit(c))
            {
                token[i] = c;
                c = getchar();
                i++;
                if(c == '.')
                {
                    c = getchar();
                    if(isdigit(c))
                    {
                        case FLOAT:
                    }
                }
                else if(c == 'e' || c == 'E')
                {
                    c = getchar();
                    if(c == '+' || c == '-' || isdigit(c))
                    {
                        case FLOAT:
                    }
                }
                else case INT:

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

        case 55:
        {
            token[i]=c;
            c = getchar();
            if(c == '=')
            {
                case LOE:
            }
            else
            {
                case LESS:
            }

        }

        case 56:
        {
            token[i]=c;
            c = getchar();
            if(c == '=')
            {
                case EQUAL:
            }
            else
            {
                case ASSIGN:
            }

        }

        case 57:
        {
            token[i]=c;
            c = getchar();
            if(c == '=')
            {
                case MOE:
            }
            else
            {
                case MORE:
            }
        }




        case STRING:break;
        case ID:break;
        case SINGLECOM:break;
        case BEGCOM:break;
        case ENDCOM:break;
    }





    return 0;
}
