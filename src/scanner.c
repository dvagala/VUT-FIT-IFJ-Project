#include <stdio.h>
#include <malloc.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include "scanner.h"
int c;

static char* Keywords[KEYWORD_COUNT]={"if","else","def","do","end","not","nil","then","while"};
tToken nextToken()
{

    char* token=malloc((sizeof(token)*50));
    tToken identificator;

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
                                if(strcmp(token, "if") == 0)
                                {
                                   case IF:
                                   {
                                        identificator.type = IF;
                                        identificator.data.string = token;
                                        break;
                                   }
                                }
                                else if(strcmp(token, "else") == 0)
                                {
                                    case ELSE:
                                    {
                                        identificator.type = ELSE;
                                        identificator.data.string = token;
                                        break;
                                    }
                                }
                                else if(strcmp(token, "def") == 0)
                                {
                                    case DEF:
                                    {
                                        identificator.type = DEF;
                                        identificator.data.string = token;
                                        break;
                                    }
                                }
                                else if(strcmp(token, "do") == 0)
                                {
                                    case DO:
                                    {
                                        identificator.type = DO;
                                        identificator.data.string = token;
                                        break;
                                    }
                                }
                                else if(strcmp(token, "end") == 0)
                                {
                                    case END:
                                    {
                                        identificator.type = END;
                                        identificator.data.string = token;
                                        break;
                                    }
                                }
                                else if(strcmp(token, "nil") == 0)
                                {
                                    case NIL:
                                    {
                                        identificator.type = NIL;
                                        identificator.data.string = token;
                                        break;
                                    }
                                }
                                else if(strcmp(token, "then") == 0)
                                {
                                    case THEN:
                                    {
                                        identificator.type = THEN;
                                        identificator.data.string = token;
                                        break;
                                    }
                                }
                                else if(strcmp(token, "while") == 0)
                                {
                                    case WHILE:
                                    {
                                        identificator.type = WHILE;
                                        identificator.data.string = token;
                                        break;
                                    }
                                }
                                else if(strcmp(token, "not") == 0)
                                {
                                    case NOT:
                                    {
                                        identificator.type = NOT;
                                        identificator.data.string = token;
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
                identificator.type=IDENTIFICATOR;
                identificator.data.string=token;
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
                            identificator.type = FLOAT;
                            identificator.data.value_double = atof(token);
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
                            identificator.type = FLOAT_EXPO;
                            identificator.data.value_double = atof(token);
                            break;
                        }
                    }
                }
                else if (!isdigit(c))
                    case INT:
                    {
                        identificator.type = INT;
                        printf("%s \n",token);
                        identificator.data.value_double = atoi(token);
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
                    identificator.type = INC;
                    break;
                }
            }
            else
                case PLUS:
                {
                    identificator.type = PLUS;
                    break;
                }
        }

        case MINUSCASES:
        {
            if (c=='-')
            {
                case DEC:
                {
                    identificator.type = DEC;
                    break;
                }
            }
            else
                case MINUS:
                {
                    identificator.type = MINUS;
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
                    identificator.type = LOE;
                    break;
                }
            }
            else
            {
                case LESS:
                {
                    identificator.type = LESS;
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
                    identificator.type = EQUAL;
                    break;
                }
            }
            else
            {
                case ASSIGN:
                {
                    identificator.type = ASSIGN;
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
                    identificator.type = MOE;
                    break;
                }
            }
            else
            {
                case MORE:
                {
                    identificator.type = MORE;
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
            identificator.type = INC;
            identificator.data.string = token;
            break;

        }

        case MULT:
        {
            identificator.type = MULT;
            break;
        }

        case MODULO:
        {
            identificator.type = MODULO;
            break;
        }

        case DIVIDE:
        {
            identificator.type = DIVIDE;
            break;
        }

        case LBRA:
        {
            identificator.type = LBRA;
            break;
        }

        case RBRA:
        {
            identificator.type = RBRA;
            break;
        }

        case SEMICOLON:
        {
            identificator.type = SEMICOLON;
            break;
        }

        case LPAR:
        {
            identificator.type = LPAR;
            break;
        }

        case RPAR:
        {
            identificator.type = RPAR;
            break;
        }

        case SINGLECOM:
        {
            identificator.type = SINGLECOM;
            break;
        }

        case ID:
        {
            identificator.type = ID;
            break;
        }
        case BEGCOM:
        {
            identificator.type = BEGCOM;
            break;
        }
        case ENDCOM:
        {
            identificator.type = ENDCOM;
            break;
        }

        case ERROR:
        {
            printf("You really need to stop.");
            break;
        }
        default:break;
    }

    token=NULL;
    free(token);
    return identificator;
}

int main()
{
    c=getchar();
    do
    {
        nextToken();
        if (isspace(c) && c!='\n' && c!=' ')
        {
            c=getchar();
            printf("caught a not newline, new char is %c",c);
        }
    } while (c != '\n');
    return 0;
}