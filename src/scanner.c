#include <stdio.h>
#include <malloc.h>
#include <ctype.h>
#include "scanner.h"
int main() {

    char token[50]={0};
    int c;

    c=getchar();
    int state;

    if (isalpha(c))
    {

        state=1;
    }

    if (isdigit(c))
    {
        state=2
    }







    return 0;
}
