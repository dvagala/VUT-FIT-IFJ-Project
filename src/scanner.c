#include "scanner.h"
int c=30000;

static char* Keywords[KEYWORD_COUNT]={"if","else","def","do","end","not","nil","then","while"};
tToken nextToken()
{

    Tokens_Types state=0;

    if (c == 30000)
    {

        c=getchar();
        if (c == '=')
        {
            state=BEGCOM;
        }
        else if (isspace(c))
        {
            while (isspace(c))
            {
                c=getchar();
                if (c == '=')
                {
                    state=BEGCOM;
                    break;
                }
            }
        }
    }
    char* token=malloc((sizeof(int)*50));
    tToken identificator;


    while (isspace(c) && c!='\n')
    {
        c=getchar();
    }
    if ((c>='a'&&c<='z') || c=='_')
    {
        state = WORD;
    }

    else if (c == '\n')
    {

        while (isspace(c))
        {

            c=getchar();
            //decide between
            if (c == '=')
            {
                state = BEGCOM;
                break;
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
        state= INT;
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
        if (state!=BEGCOM)
        {
            state = ASSIGN;
        }
    }
    else if(c == '{')
    {
        state = LBRA;
    }
    else if(c == '}')
    {
        state = RBRA;
    }
    else if(c == ',')
    {
        state = COLON;
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
        state = LESS;
    }
    else if(c == '>')
    {
        state = MORE;
    }
    else if (c == '\"')
    {
        state = STRING;
    }
    else if (c == '!')
    {
        state = NOTEQUAL;
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
                        if (c==' '||c=='\n'||isspace(c)||c==EOF)
                        {
                            case KEYWORDCASES: //100 is word
                            {
                                identificator.type=j;
                                token=NULL;
                                free(token);
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
                    c=getchar();
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
                                else if (isdigit(c))
                                {
                                    while(isdigit(c))
                                    {
                                        token[i] = c;
                                        i++;
                                        c = getchar();
                                    }
                                }
                                else
                                {
                                    identificator.type=ERROR;
                                    free(token);
                                    return identificator;

                                }
                            }
                            else if (isdigit(c))
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
                    token[i]=c;
                    i++;
                    c = getchar();
                    if(c == '+' || c == '-')
                    {
                        case FLOAT_EXPO:
                        {
                            token[i]=c;
                            i++;
                            c=getchar();
                            if (!isdigit(c))
                            {
                                free(token);
                                identificator.type=ERROR;
                                break;

                            }
                            while(isdigit(c))
                            {
                                token[i]=c;
                                i++;
                                c=getchar();
                            }
                            identificator.type=FLOAT;
                            identificator.data.value_double=atof(token);
                            break;
                        }
                    }
                    else if (isdigit(c))
                    {
                        while (isdigit(c))
                        {
                            token[i]=c;
                            i++;
                            c=getchar();
                        }
                        identificator.type=FLOAT;
                        identificator.data.value_double=atof(token);
                        break;
                    }

                }
                else if (!isdigit(c))
                    case INT:
                    {
                        if (!isalpha(c))
                        {
                            identificator.type = INT;
                            identificator.data.value_int = atoi(token);
                        }
                        else
                        {
                            identificator.type = ERROR;
                        }
                        break;
                    }

            }break;
        }

        case LESS:
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
                    identificator.type = LESS;
                    break;
            }

        }

        case ASSIGN:
        {
            c = getchar();
            if(c == '=')
            {
                case EQUAL:
                {
                    c = getchar();
                    identificator.type = EQUAL;
                    break;
                }
            }

            else
            {
                    identificator.type = ASSIGN;
                    break;
            }

        }

        case MORE:
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
                    identificator.type = MORE;
                    break;
            }
        }

        case NOTEQUAL:
        {
            c=getchar();
            if (c == '=')
            {
                c=getchar();
                identificator.type=NOTEQUAL;
                break;
            }
            else
                identificator.type=ERROR;
            break;
        }

        case STRING:
        {
            int mult_alloc=1;
            c=getchar();
            char hexValue[3]={0};
            int symbol;
            while (c !='\"')
            {
                if (i==49*mult_alloc && i!=0)
                {
                    mult_alloc++;
                    token=malloc(sizeof(char)*49*mult_alloc);
                }
                if (c == '\\')
                {
                    c=getchar();
                    switch(c)
                    {
                        case 'n':
                        {
                            token[i]='\n';
                            i++;
                            break;
                        }
                        case 't':
                        {
                            token[i]='\t';
                            i++;
                            break;
                        }
                        case 's':
                        {
                            token[i]=' ';
                            i++;
                            break;
                        }
                        case '\\':
                        {
                            token[i]='\\';
                            i++;
                            break;
                        }
                        case '\"':
                        {
                            token[i]='\"';
                            i++;
                            break;
                        }
                        case 'x':
                        {
                            c=getchar();
                            if (isdigit(c)||(c>='A'&&c<='F'))
                            {
                                hexValue[0]=c;
                                c=getchar();
                                if (isdigit(c)||(c>='A'&&c<='F'))
                                {
                                    hexValue[1]=c;
                                }
                                else
                                {
                                    token[i]=c;
                                    c++;
                                }
                                hexValue[2]='\0';
                                symbol = strtol(hexValue,NULL,16);
                                token[i]=symbol;
                                i++;

                                break;
                            }
                            else
                            {
                                identificator.type=ERROR;
                                token=NULL;
                                free(token);
                                return identificator;
                            }
                        }
                        default:
                        {
                            identificator.type=ERROR;
                            token=NULL;
                            free(token);
                            return identificator;
                        }
                    }
                    c=getchar();
                }
                else
                {
                    token[i] = c;
                    i++;
                    c = getchar();
                }
                if (c == EOF)
                {
                    identificator.type=ERROR;
                    token=NULL;
                    free(token);
                    return identificator;
                }
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

        case COLON:
        {
            c=getchar();
            identificator.type = COLON;
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

            while(c!='\n' && c!=EOF)
            {
                c=getchar();
            }
            c=getchar();
            while (isspace(c))
            {
                c=getchar();
            }
            if (c !='=')
                return nextToken();
            //identificator.type = SINGLECOM;
            //break;
        }

        case BEGCOM:
        {
            char start[6]={0};
            char end[5]={0};
            for (int x=0;x<5;x++)
            {
                c=getchar();
                start[x]=c;
                i++;
            }

            if (strcmp(start,"begin")==0)
            {
                c=getchar();
                if (!isspace(c))
                {
                    identificator.type=ERROR;
                }
                else
                {
                    int isEnd=0;
                    while (!(isEnd==1 && (c=='\n'||c==EOF)))
                    {
                        int corrector=0;

                        if (c=='\n' && isEnd==0)
                        {
                            for (int e=0;e<4;e++)
                            {
                                c=getchar();
                                if (c=='\n')
                                {
                                    e=-1;
                                }
                                end[e]=c;
                            }

                            if (strcmp(end,"=end")==0)
                            {
                                c=getchar();
                                corrector=1;
                                if (isspace(c)||c==EOF)
                                {
                                    isEnd=1;
                                }

                            }
                        }
                        if (corrector==0)
                        {
                            c = getchar();
                        }
                    }
                    c=getchar();
                    //identificator.type=BEGCOM;
                    return nextToken();
                }
                ;
            }
            else
            {
                identificator.type=ERROR;
            }
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
            identificator.type=ERROR;
            c=getchar();
            break;
        }
        default:break;
    }

    //printf("HERE IS TOKEN %s",token);
    token=NULL;
    free(token);
    return identificator;
}

int main()
{

    for (int i=0;i<10;i++)
    {
        tToken token =nextToken();
        if (i==0)
        {
         //  printf("%s \n",token.data.string);
        }
        printf("Token type: %d \n",token.type);
    }
    return 0;
}