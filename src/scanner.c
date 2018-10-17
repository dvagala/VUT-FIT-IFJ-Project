#include <stdio.h>
#include <malloc.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include "scanner.h"


static char* Keywords[KEYWORD_COUNT]={"IF","ELSE","DEF","DO","END","NOT","NIL","THEN","WHILE"};
int next(int c)
{

    char token[50]={0};

    Tokens_Types state=0;


    while (c == ' ' || isspace(c))
    {
        c=getchar();
    }
    if (isalpha(c) && !isspace(c) && c!=' ')
    {
        state = WORD;
    }

    else if (isdigit(c))
    {
        state= NUMBER;
    }

    else if(c == '+')
    {
        state = PLUSCASES;
    }
    else if(c == '-')
    {
        state = MINUSCASES;
    }
    else if(c == '*')
    {
        state = MULT;
    }
    else if(c == '%')
    {
        state = MODULO;
    }
    else if(c == '/')
    {
        state = DIVIDE;
    }
    else if(c == '=')
    {
        state = EQUALCASES;
    }
    else if(c == '{')
    {
        state = LBRA;
    }
    else if(c == '}')
    {
        state = RBRA;
    }
    else if(c == ';')
    {
        state = SEMICOLON;
    }
    else if(c == '(')
    {
        state = LPAR;
    }
    else if(c == ')')
    {
        state = RPAR;
    }
    else if(c == '<')
    {
        state = LESSCASES;
    }
    else if(c == '>')
    {
        state = MORECASES;
    }
    else if (c == '\"')
    {
        state = STRING;
    }

    else if (c == '#')
    {
        state = SINGLECOM;
    }

    else
    {
        state =ERROR;
    }
    int i = 0;

    switch(state)
    {

        case WORD://IDENTIFICATOR OR A KEY WORD
        {

            while (isalpha(c) || isdigit(c) || c=='_')
            {
                token[i] = c;
                c = getchar();
                i++;
                for (int j=0;j<KEYWORD_COUNT;j++)
                {
                    if(strcmp(token,Keywords[j])==0)
                    {
                        c=getchar();
                        if (c==' '||c=='\n')
                        {
                            case KEYWORDCASES: //100 is word
                            {
                                if(strcmp(token, "IF") == 0)
                                {
                                   case IF:
                                   {
                                        break;
                                   }
                                }
                                else if(strcmp(token, "ELSE") == 0)
                                {
                                    case ELSE:
                                    {
                                        break;
                                    }
                                }
                                else if(strcmp(token, "DEF") == 0)
                                {
                                    case DEF:
                                    {
                                        break;
                                    }
                                }
                                else if(strcmp(token, "DO") == 0)
                                {
                                    case DO:
                                    {
                                        break;
                                    }
                                }
                                else if(strcmp(token, "END") == 0)
                                {
                                    case END:
                                    {
                                        break;
                                    }
                                }
                                else if(strcmp(token, "NIL") == 0)
                                {
                                    case NIL:
                                    {
                                        break;
                                    }
                                }
                                else if(strcmp(token, "THEN") == 0)
                                {
                                    case THEN:
                                    {
                                        break;
                                    }
                                }
                                else if(strcmp(token, "WHILE") == 0)
                                {
                                    case WHILE:
                                    {
                                        break;
                                    }
                                }
                                else if(strcmp(token, "NOT") == 0)
                                {
                                    case NOT:
                                    {
                                        break;
                                    }
                                }
                                else break;
                            }
                        }
                    }
                }
            }

            case IDENTIFICATOR:
            {
                if (c == '?' || c == '!')
                {
                    token[i] = c;
                }
                printf("CONGRATS! you found: %s\n",token);
                break;
            }
    } //CASE FOR FLOAT AND INT
        case NUMBER:
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
                        {
                            break;
                        }
                    }
                }
                else if(c == 'e' || c == 'E')
                {
                    c = getchar();
                    if(c == '+' || c == '-')
                    {
                        case FLOAT_EXPO:
                        {
                            break;
                        }
                    }
                }
                else if (!isdigit(c))
                    case INT:
                    {
                        printf("here we have a number, and it's %s \n",token);
                        break;
                    }

            }
        }

        case PLUSCASES:
        {
            token[i]=c;
            c = getchar();
            if (c=='+')
            {
                case INC:
                {
                    break;
                }
            }
            else
                case PLUS:
                {
                    break;
                }
        }

        case MINUSCASES:
        {
            if (c=='-')
            {
                case DEC:
                {
                    break;
                }
            }
            else
                case MINUS:
                {
                    break;
                }
        }

        case LESSCASES:
        {
            token[i]=c;
            c = getchar();
            if(c == '=')
            {
                case LOE:
                {
                    break;
                }
            }
            else
            {
                case LESS:
                {
                    break;
                }
            }

        }

        case EQUALCASES:
        {
            token[i]=c;
            c = getchar();
            if(c == '=')
            {
                case EQUAL:
                {
                    break;
                }
            }
            else
            {
                case ASSIGN:
                {
                    printf("Hey!\n");
                    break;
                }
            }

        }

        case MORECASES:
        {
            token[i]=c;
            c = getchar();
            if(c == '=')
            {
                case MOE:
                {
                    break;
                }
            }
            else
            {
                case MORE:
                {
                    break;
                }
            }
        }

        case STRING:
        {
            c=getchar();
            while (c!='\"')
            {
                c=getchar();
                token[i]=c;
                i++;
            }
            break;

        }

        case MULT:
        {
            break;
        }

        case MODULO:
        {
            break;
        }

        case DIVIDE:
        {
            break;
        }

        case LBRA:
        {
            break;
        }

        case RBRA:
        {
            break;
        }

        case SEMICOLON:
        {
            break;
        }

        case LPAR:
        {
            break;
        }

        case RPAR:
        {
            break;
        }

        case SINGLECOM:
        {
            break;
        }

        case ID:
        {
            break;
        }
        case BEGCOM:
        {
            break;
        }
        case ENDCOM:
        {
            break;
        }

        case ERROR:
        {
            printf("You really need to stop.");
            break;
        }
        default:break;
    }



    return c;
}

int main()
{
    int c;
    c=getchar();
    do
    {
        c=next(c);
        if (isspace(c) && c!='\n' && c!=' ')
        {
            c=getchar();
            printf("caught a not newline, new char is %c",c);
        }
    } while (c != '\n');
    return 0;
}