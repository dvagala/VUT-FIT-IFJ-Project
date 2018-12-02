/*
 * Project: Implementace překladače imperativního jazyka IFJ18
 *
 * File: scanner.c
 *
 * xglosk00   Kristian Glos
 * xvagal00   Dominik Vagala
 * xvinsj00   Jakub Vins
 * xabike00   Zhamilya Abikenova
 *
 */


#include "scanner.h"
int c=30000; //global variable c, holding current character. We need to hold this value between function callings,
// and setting is as global is the simplest solution.

// Set to '1', if you want to print debug stuff
#define DEBUG_SCANNER 0

/*
This is a function that saves characters scanned from code into a string. This string is dynamically allocated so we always
 need to be sure to have enough space for the characters.
*/
void bufferToken(tokenstringptr ts)
{
    if ((ts->index/ts->chunk_amount)==CHUNK)
    {
        ts->chunk_amount=(ts->chunk_amount)+1;
        ts->string=realloc(ts->string,sizeof(char)*CHUNK*(ts->chunk_amount));
    }
    ts->string[ts->index]=c;
    ts->index=(ts->index)+1;
    ts->string[ts->index]='\0';
}

void freeString(tokenstringptr ts)
{
    ts->string=NULL;
    free(ts->string);
    free(ts);
}
static char* Keywords[KEYWORD_COUNT]={"if","else","def","do","end","not","nil","then","while"};
tToken nextToken()
{
    Tokens_Types state=0;

    if (c == 30000) //if we have yet to read a single character from given code:
    {
        c=getchar();
        if (c == '=') //if it's =, the code starts with a multiline comment.
        {
            state=BEGCOM;
        }
    }

    //allocating string space for cases STRING, INT, FLOAT, IDENTIFICATOR. Doing this through a struct that has necessary elements
    //to do all the handling of dynamic allocation.
    tokenstringptr TokenString = malloc(sizeof(struct tokenstring));
    TokenString->string=NULL;
    TokenString->string=malloc(sizeof(char)*CHUNK);
    TokenString->index=0;
    TokenString->chunk_amount=1;
    tToken identificator;

    //Getting rid of all whitespace characters between tokens
    while (isspace(c) && c!='\n')
    {
        c=getchar();
    }

    if ((c>='a'&&c<='z') || c=='_')
    {
        state = WORD;
    }
///AN EXTENSIVE IF ELSE CONDITION CHECKING ALL POSSIBLE ROUTES FOR TOKEN SCANNING
    else if (c == '\n')
    {
            c=getchar();
            //decide between
            if (c == '=')
            {
                state = BEGCOM;
            }
            else
            {
                state = EOL_CASE;
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

    switch(state)
    {
        case WORD://IDENTIFICATOR OR A KEY WORD
        {
            int f=0;
            while (isalpha(c) || isdigit(c) || c=='_')
            {

                f++;
                bufferToken(TokenString);
                c = getchar();

                for (int j=0;j<KEYWORD_COUNT;j++)
                {
                    if(strcmp(TokenString->string,Keywords[j])==0) //string is compared to an array of keywords
                    {
                        if (!(isalpha(c)||isdigit(c)||c=='_'))  //if it was a keyword, and it is followed by anything that
                                                                //can be used to extend identificator, name the keyword
                        {
                            case KEYWORDCASES: //Therefore no need for token string, free it and return keyword.
                            {
                                identificator.type=j;
                                return identificator;
                            }
                        }
                    }
                }
            }
            case IDENTIFICATOR: //in all other cases, it is an ID
            {
                if (c == '?' || c == '!') //the IDs of functions can end in either, thus the condition.
                {
                    bufferToken(TokenString);
                    c=getchar();
                }
                identificator.type=IDENTIFICATOR;
                identificator.data.string=TokenString->string;
                break;
            }
        } //CASE FOR FLOAT AND INT
        case INT:
        {

            while (isdigit(c)) //repeat until you hit a non-digit character
            {
                bufferToken(TokenString);
                c=getchar();
            }

            if (c == '.') //if it is a dot, we set type as float, because it can no longer be an integer
            {
                identificator.type=FLOAT;
                bufferToken(TokenString);
                c=getchar();
                if (isdigit(c))//There has to be atleast ONE digit following the dot, otherwise we'll return ERROR
                {
                    while (isdigit(c)) //we collect all digits after dot
                    {
                        bufferToken(TokenString);
                        c=getchar();
                    }
                }
                else
                {
                    identificator.type=ERROR;
                }
            }
            if (c=='e' || c == 'E') //and start the exponent part of float, or end scanning
            {
                if (identificator.type != ERROR)
                {
                    identificator.type = FLOAT;
                }
                bufferToken(TokenString);
                c=getchar();
                if (c=='+' || c=='-') //exponent can be followed by non-required + or -
                {
                    bufferToken(TokenString);
                    c=getchar();
                }
                if (isdigit(c)) //Or atleast one digit in either case. Otherwise raise error.
                {
                    while(isdigit(c))
                    {
                        bufferToken(TokenString);
                        c=getchar();
                    }
                }
                else
                {
                    identificator.type=ERROR;
                }
            }


            if (identificator.type == ERROR) //if we raised error, free our resources and return
            {
                freeString(TokenString);
                return identificator;
            }
            else if (identificator.type == FLOAT)//if we found float, we return value saved as a double
            {
                identificator.data.value_double=atof(TokenString->string);
                freeString(TokenString);
                break;
            }
            else //otherwise we stayed in integer, so return that.
            {
                identificator.type=INT;
                identificator.data.value_int=atoi(TokenString->string);
                freeString(TokenString);
                break;
            }
        }

        case LESS: //< can be either followed by = to return LESS OR EQUAL, or simply LESS
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

        case ASSIGN: //can be followed by = for EQUAL
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

        case MORE: //can be followed by = for MORE OR EQUAL
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

        case NOTEQUAL: // !can ONLY be followed by =, otherwise we raise error
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

        case STRING: //STRING CASE
        {
            c=getchar();
            char hexValue[3]={0}; //special array to save HEX value that follows after sequence '\x'
            int symbol; //we'll save this value in int
            int tookFlag=0; //variable used to take care of edge case where string ends with \xh"
            int bufferedMore=0;
            while (c !='\"')
            {
                if (c == '\n')
                {
                    identificator.type=ERROR;
                    freeString(TokenString);
                    return identificator;
                }
                if (c == '\\') // \ is the so called escape character for special symbols. For our case, we make a case for each and implement
                {              //their proper behavior.
                    c=getchar();
                    switch(c)
                    {
                        case 'n': //n is newline
                        {
                            TokenString->string[TokenString->index]='\n';
                            (TokenString->index)++;
                            break;
                        }
                        case 't': //t is tab
                        {
                            TokenString->string[TokenString->index]='\t';
                            (TokenString->index)++;
                            break;
                        }
                        case 's': //s is space
                        {
                            TokenString->string[TokenString->index]=' ';
                            (TokenString->index)++;
                            break;
                        }
                        case '\\': // is escaped escape character
                        {
                            TokenString->string[TokenString->index]='\\';
                            (TokenString->index)++;
                            break;
                        }
                        case '\"': // is escaped "
                        {
                            TokenString->string[TokenString->index]='\"';
                            (TokenString->index)++;
                            break;
                        }
                        case 'x': //is a hexadecimal value of a character in the ASCII table
                        {
                            c=getchar();
                            if (isdigit(c)||(c>='A'&&c<='F')||(c>='a' && c<='f')) //hexadecimal number follows atleast ONCE, otherwise error
                            {
                                hexValue[0]=c; //we save that character in hexvalue array, which we'll later translate to a hexadecimal int value
                                c=getchar();
                                if (isdigit(c)||(c>='A'&&c<='F')||(c>='a'&&c<='f')) //same for a second character, if applicable
                                {
                                    hexValue[1]=c;
                                }
                                else //however not required
                                {
                                    hexValue[1]='\0';
                                    if (c=='"') //if the second character was terminating one, we'll need to save it's occurence
                                    {
                                        tookFlag=1;
                                    }
                                    else
                                    {
                                        bufferedMore=1;
                                    }
                                }
                                hexValue[2]='\0';
                                symbol = strtol(hexValue,NULL,16); //converting the value
                                TokenString->string[TokenString->index]=symbol;
                                (TokenString->index)++; //then putting in that value as a character in our array (gets converted to char) afterwards
                                if (bufferedMore==1)
                                {

                                    bufferToken(TokenString);
                                }
                                break;
                            }
                            else
                            {
                                identificator.type=ERROR;
                                freeString(TokenString);
                                return identificator;
                            }
                        }
                        default: //if \ wasn't followed by either, raise ERROR
                        {
                            identificator.type=ERROR;
                            freeString(TokenString);
                            return identificator;
                        }
                    }
                    if (tookFlag==1) //and jump out of cycle to not loop
                    {
                        break;
                    }
                    c=getchar();
                }
                else
                {
                    bufferToken(TokenString);
                    c=getchar();
                }
                if (c == EOF) //if we found EOF without " it means that string was never terminated and we raise ERROR
                {
                    identificator.type=ERROR;
                    freeString(TokenString);
                    return identificator;
                }
            } //cycle will be repeated until we reach an unescaped " after that string will be saved and token returned.
            c=getchar();
            identificator.type = STRING;
            identificator.data.string = TokenString->string;
            break;

        }
///Simple cases for one character tokens (arithmetic operations, and such).
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
        //SINGLE COMMENT case gathers all characters until either NEWLINE, or End of File values.
        case SINGLECOM:
        {

            while(c!='\n' && c!=EOF)
            {
                c=getchar();
            }

                return nextToken();
        }
        //BEGCOM, MULTILINE OR BLOCK COMMENT, can only start on the very start of a new line. (or start of the code.)
        case BEGCOM://starting by = at the start of the line
        {
            char start[6]={0};
            char end[5]={0};
            for (int x=0;x<5;x++)
            {
                c=getchar();
                start[x]=c;
            }

            if (strcmp(start,"begin")==0) //it is either followed exactly by "begin", or we raise ERROR
            {
                c=getchar();
                if (!isspace(c))
                {
                    identificator.type=ERROR;
                }
                else
                {
                    int isEnd=0;
                    while (!(isEnd==1 && (c=='\n'||c==EOF))) //we scan all characters, until we find a string "=end" followed by whitespace
                    {                                        // on the start of a line and then all characters on the rest of that line.
                        int corrector=0; //value corrector exists for proper scanning of content at the start of the line,
                                         // which we need to scan separatedly looking for =end
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

                            if (strcmp(end,"=end")==0) //if we scanned end, we moved to the next state in cycle
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

                        if (isEnd==0 && c==EOF) //if we reached the End of File without ever properly ending the comment, raise an error.
                        {
                            identificator.type=ERROR;
                            freeString(TokenString);
                            return identificator;
                        }
                    }
                    return nextToken();
                }
            }
            else
            {
                identificator.type=ERROR;
            }
            break;
        }

        case EOL_CASE: //case for newline
        {
            identificator.type=EOL_CASE;
            break;
        }

        case EOF_CASE: //case for end of file.
        {
            identificator.type=EOF_CASE;
            break;
        }
        case ERROR: //natural error case, if there's an unknown character in the code.
        {
            identificator.type=ERROR;
            c=getchar();
            break;
        }
        default:break;
    }

    //free the token variable, as we saved it already into struct identificator, and return the token.
    return identificator;
}
