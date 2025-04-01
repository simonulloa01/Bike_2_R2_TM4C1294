#include <stdio.h>

// Functions to test
int addNums(int a, int b);

int main ()
{
    int sum = addNums(4, 5);
    printf("Sum: %d\n", sum);
    return 0;
}

int addNums(int a, int b)
{
    return a + b;
}