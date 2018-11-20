#include <stdio.h>
#include <malloc.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include "scanner.h"
int c=30000;

static char* Keywords[KEYWORD_COUNT]={"if","else","def","do","end","not","nil","then","while"};
tToken nextToken()
{

    if (c == 30000)
    {
        c=getchar();
    }
    char* token=malloc((sizeof(int)*50));
    tToken identificator;
    Tokens_Types state=0;

    while (isspace(c) && c!='\n')
    {
        c=getchar();
    }
    if (isalpha(c))
    {
        state = WORD;
    }

    else if (c == '\n')
    {
        while (c == '\n')
        {
            c=getchar();
            if (c == '=')
            {
                printf("am I here?");
                state = BEGCOM;
            }
            else
            {
                state = EOL_CASE;
            }
        }
    }

    else if (c == EOF)
    {
        state = EOF_CASE;
    }

    else if (isdigit(c))
    {
        state= NUMBER;
    }

    else if(c == '+')
    {
        state = PLUS;
    }
    else if(c == '-')
    {
        state = MINUS;
    }
    else if(c == '*')
    {
        state = MULT;
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
            int f=0;
            while (isalpha(c) || isdigit(c) || c=='_')
            {
                f++;
                token[i] = c;
                c = getchar();
                i++;
                for (int j=0;j<KEYWORD_COUNT;j++)
                {
                    if(strcmp(token,Keywords[j])==0)
                    {
                        c=getchar();
                        if (c==' '||c=='\n'||isspace(c)||c==EOF)
                        {
                            case KEYWORDCASES: //100 is word
                            {
                                identificator.type=j;
                                free(token);
                                token=NULL;
                                return identificator;
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
                i++;
                c = getchar();
                if(c == '.')
                {
                    token[i]=c;
                    i++;
                    c = getchar();

                    if(isdigit(c))
                    {
                        case FLOAT:
                        {
                            token[i]=c;
                            i++;
                            c=getchar();

                            if (c=='e' || c=='E')
                            {
                                token[i]=c;
                                i++;
                                c=getchar();
                                if (c=='+'||c=='-')
                                {
                                    token[i]=c;
                                    i++;
                                    c=getchar();
                                    if (isdigit(c))
                                    {
                                        while(isdigit(c))
                                        {
                                            token[i] = c;
                                            i++;
                                            c=getchar();
                                        }
                                    }
                                    else
                                    {
                                        identificator.type=ERROR;
                                        free(token);
                                        return identificator;

                                    }
                                }
                                else
                                {
                                    identificator.type=ERROR;
                                    free(token);
                                    return identificator;

                                }
                            }
                            if (isdigit(c))
                            {
                                while (isdigit(c))
                                {
                                    token[i] = c;
                                    i++;
                                    c = getchar();
                                }
                            }
                            identificator.type = FLOAT;
                            identificator.data.value_double = atof(token);
                            printf("%f is here",identificator.data.value_double);
                            break;
                        }

                    }
                    else
                    {
                        free(token);
                        identificator.type=ERROR;
                        return identificator;
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
                            c=getchar();
                            if (!isdigit(c))
                            {
                                free(token);
                                identificator.type=ERROR;
                                return identificator;
                            }
                            while(isdigit(c))
                            {
                                token[i]=c;
                                i++;
                                c=getchar();
                            }
                            break;
                        }
                    }
                }
                else if (!isdigit(c))
                    case INT:
                    {
                        printf("did I get here?");
                        identificator.type = INT;
                        identificator.data.value_int = atoi(token);
                        break;
                    }

            }break;
        }

        case LESSCASES:
        {
            c = getchar();
            if(c == '=')
            {
                case LOE:
                {
                    c=getchar();
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
            c = getchar();
            if(c == '=')
            {
                case EQUAL:
                {
                    c=getchar();
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
            c = getchar();
            if(c == '=')
            {
                case MOE:
                {
                    c=getchar();
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
            while (c !='\"')
            {
                token[i] = c;
                i++;
                c=getchar();
            }
            token[i]='\0';
            c=getchar();
            identificator.type = STRING;
            identificator.data.string = token;
            break;

        }

        case MULT:
        {
            identificator.type = MULT;
            c=getchar();
            break;
        }

        case PLUS:
        {
            identificator.type = PLUS;
            c=getchar();
            break;
        }

        case MINUS:
        {
            identificator.type = MINUS;
            c=getchar();
            break;
        }
        case DIVIDE:
        {
            identificator.type = DIVIDE;
            c=getchar();
            break;
        }

        case LBRA:
        {
            c=getchar();
            identificator.type = LBRA;
            break;
        }

        case RBRA:
        {
            c=getchar();
            identificator.type = RBRA;
            break;
        }

        case SEMICOLON:
        {
            c=getchar();
            identificator.type = SEMICOLON;
            break;
        }

        case LPAR:
        {
            c=getchar();
            identificator.type = LPAR;
            break;
        }

        case RPAR:
        {
            c=getchar();
            identificator.type = RPAR;
            break;
        }

        case SINGLECOM:
        {
            identificator.type = SINGLECOM;
            while(c!='\n' && c!=EOF)
            {
                c=getchar();
            }
            c=getchar();

            break;
        }

        case BEGCOM:
        {
            printf("are we here?");
            char start[6]={0};
            char end[5]={0};
            for (int x=0;x<5;x++)
            {
                start[i]=getchar();
                i++;
            }
            printf(">>%s<<",start);
            if (strcmp(start,"begin")==0)
            {
                printf("success");
                int isEnd=0;
                while (!(isEnd==1 && c=='\n'))
                {
                    c=getchar();
                    printf("%c",c);
                    if (c=='\n')
                    {
                        printf("hello");
                        for (int e=0;e<4;e++)
                        {
                            c=getchar();
                            end[e]=c;
                        }

                        if (strcmp(end,"=end")==0)
                        {
                            printf("success?");
                            isEnd=1;
                        }
                    }
                }
            }
            nextToken();
            break;
        }
        case ENDCOM:
        {
            identificator.type = ENDCOM;
            break;
        }

        case EOL_CASE:
        {
                identificator.type=EOL_CASE;
            break;
        }

        case EOF_CASE:
        {
            identificator.type=EOF_CASE;
            break;
        }
        case ERROR:
        {
            printf("You really need to stop.");
            c=getchar();
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

    for (int i=0;i<10;i++)
    {
        tToken token =nextToken();
        printf("Token type: %d \n",token.type);
    }
    return 0;
}