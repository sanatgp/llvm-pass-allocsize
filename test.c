// test.c
#include <stdio.h>

int calculate(int a, int b) {
    int result;
    if (a > b) {
        result = a - b;
    } else {
        result = a + b;
    }
    return result;
}

int main() {
    int x[10];
    int sum = 0;

    for (int i = 0; i < 10; i++) {
        x[i] = i * 2;
        sum += x[i];
    }

    int result1 = calculate(sum, 15);
    int result2 = calculate(sum, 25);

    printf("Result 1: %d\n", result1);
    printf("Result 2: %d\n", result2);

    return 0;
}

