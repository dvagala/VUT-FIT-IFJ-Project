#include <stdio.h>
#include "scanner.h"

int main() {


    int c;
    while ((c=getchar())!='\n')
    {
        printf("%c",c);
    }
    return 0;
}
