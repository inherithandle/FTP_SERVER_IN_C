#include <stdio.h>
int main()
{
    int a = 0;
    int b = 2;
    int result;
    result = a && ++b;
    printf("a = %d, b = %d, result = %d\n", a, b, result);
    result = a || ++b;
    printf("a = %d, b = %d, result = %d\n", a, b, result);
    return 0;
}