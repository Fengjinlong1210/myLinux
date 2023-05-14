/*************************************************************************
  > File Name: main.c
  > Author: Fjl
  > Mail: 239383772@qq.com 
  > Created Time: 2023-05-14 08:59:43
 ************************************************************************/

#include<stdio.h>
#include "add.h"
#include "sub.h"

int main()
{
    int a = 100;
    int b = 50;
    int c = Add(a, b);
    int d = Sub(a, b);
    printf("%d + %d = %d\n", a, b);
    printf("%d - %d = %d\n", a, b);

    return 0;
}
