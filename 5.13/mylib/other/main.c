#include <stdio.h>
#include "add.h"
#include "sub.h"

int main()
{
    int a = 10;
    int b = 20;
    printf("%d + %d = %d\n", a, b, Add(a, b));
    printf("%d - %d = %d\n", a, b, Sub(a, b));
    return 0;
}
