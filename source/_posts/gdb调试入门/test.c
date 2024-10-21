#include<stdio.h>


int sum(int a, int b) {
    a += 1;
    a += 2;
    a += b;

    return a + b;
}


int main() {
    int c = 0;
    c = sum(3, 9);

    printf("sum = %d", c);
}