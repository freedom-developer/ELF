#include "add.h"

#include <stdio.h>


int main(int argc, char **argv)
{
    int a, b;
    
    a = 1;
    b = 2;
    
    printf("%d + %d = %d\n", a, b, addii(a, b));

    return 0;
}