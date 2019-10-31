#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

int fib_rec(int n) {
    if (n == 0) {
        return 0;
    }
    if (n == 1) {
        return 1;
    }
    return fib_rec(n - 1) + fib_rec(n - 2);
}

uint64_t fib_iter(int n) {
    uint64_t prev_num = 0;
    uint64_t next_num = 1;
    for (int i = 0; i < n - 2; i++) {
        uint64_t temp = prev_num + next_num;
        prev_num = next_num;
        next_num = temp;
    }
    return prev_num + next_num;
}

uint64_t fib_memo(int n, uint64_t *fib_arr) {
    if (n == 0) {
        return 0;
    }
    if (n == 1) {
        return 1;
    }
    if (fib_arr[n] != 0) {
        return fib_arr[n];
    }
    fib_arr[n] = fib_memo(n - 1, fib_arr) + fib_memo(n - 2, fib_arr);
    return fib_arr[n];
}

int main(int argc, char **argv) {
    clock_t start;
    double elapsed = 0;
    int fib_num_rec = 0;
    uint64_t fib_num = 0;
    int iterations = 0;

    // for fib1(38) Using recursive
    start = clock();
    while (((clock() - start) / (double)CLOCKS_PER_SEC) < 0.5) {
        fib_num_rec = fib_rec(38);
        iterations += 1;
    }
    elapsed = ((((clock() - start) / (double)CLOCKS_PER_SEC)) / (double)iterations);
    printf("fib1(38) got %d and took %.6f ms per iteration\n", fib_num_rec, elapsed * 1000);

    // For fib2(90) using iterative
    iterations = 0;
    start = clock();
    while (((clock() - start) / (double)CLOCKS_PER_SEC) < 0.5) {
        for (int i = 0; i < 100; i++) {
            fib_num = fib_iter(90);
        }
        iterations += 100;
    }
    elapsed = (((clock() - start) / (double)CLOCKS_PER_SEC) / (double)iterations);
    printf("fib2(90) got %ld and took %.6f ms per iteration\n", fib_num, elapsed * 1000);

    // For fib3(90) using memo
    iterations = 0;
    start = clock();
    uint64_t *fib_arr = calloc(sizeof(*fib_arr) * (90 + 1), sizeof(uint64_t));
    while (((clock() - start) / (double)CLOCKS_PER_SEC) < 0.5) {
        for (int i = 0; i < 100; i++) {
            fib_num = fib_memo(90, fib_arr);
        }
        iterations += 100;
    }
    free(fib_arr);
    elapsed = (((clock() - start) / (double)CLOCKS_PER_SEC) / (double)iterations);
    printf("fib3(90) got %ld and took %.6f ms per iteration\n", fib_num, elapsed * 1000);
    return 0;
}
