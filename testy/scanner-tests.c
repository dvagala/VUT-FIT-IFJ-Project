#include "scanner.h"

int main()
{

    for (int i=0;i<40;i++)
    {
        tToken token =nextToken();
        if (i==0)
        {
         //  printf("%s \n",token.data.string);
        }
        printf("TokenT: %d \n",token.type);

        if (token.type == EOF_CASE)
        {
            break;
        }
        if (token.type == ERROR)
        {
            printf("----------->ERROR \n");
        }

        if (token.type == 1004)
        {
            printf("----------->ID %s \n",token.data.string);
        }
        else if (token.type == 1002)
        {
            printf("----------->STRING %s \n",token.data.string);
        }
        else if (token.type ==1000)
        {
            printf("----------->INT %d \n",token.data.value_int);
        }
    }
    return 0;
}