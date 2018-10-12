#include <stdio.h>
#include "scanner.h"

int main() {


    while ((c=getchar)!='\n')
    {
        printf("%c",c);
    }
    return 0;
}
