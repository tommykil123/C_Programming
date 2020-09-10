#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "hashfunction.h"

typedef struct test_entry {
    uint8_t *data;
    int n;
} test_entry_t;

void evaluate_hash_reduce(int n_entries, test_entry_t *entries,
                          uint32_t (*hash_f)(uint8_t *, int), uint32_t (*reduce_f)(uint32_t)) {
    clock_t start = clock();
    int collisions = 0;
    int run_num = 0;
    while (((double)(clock() - start) / CLOCKS_PER_SEC) < 0.5) {
        uint16_t table[TABLE_SIZE] = {0};
        for (int i = 0; i < n_entries; i++) {
            uint32_t hash = reduce_f(hash_f(entries[i].data, entries[i].n));
            if (table[hash] == 0) {
                table[hash] = 1;
            } else {
                collisions += 1;
            }
        }
        run_num += 1;
    }
    double time_sec = ((double)(clock() - start) / CLOCKS_PER_SEC);
    double per_iter = (time_sec / (run_num * n_entries)) * pow(10, 9);
    printf("%.2fns per iteration, with %d collisions\n", per_iter, collisions / run_num);
}

int main(int argc, char **argv) {
    // First we need to collect all the entries/strings/data that we will try to hash
    // This needs to be done up front for the benchmarking later to be valid.
    int max_entries = TABLE_SIZE / 2;
    int n_entries = 4096;
    test_entry_t *entries = calloc(max_entries, sizeof(test_entry_t));
    for (uint16_t i = 0; i < 1000; i++) {
        uint8_t *data = malloc(sizeof(i));
        memcpy(data, &i, sizeof(i));
        // printf("%d, %d\n", data[0], data[1]);
        entries[i].data = data;
        entries[i].n = 2;
    }
    FILE *f = fopen("book.txt", "r");
    if (!f) {
        perror("Failed to open book.txt");
        return 1;
    }
    for (int i = 1000; i < n_entries; i++) {
        char *str_buffer = malloc(WORD_MAX_LEN);
        if (fgets(str_buffer, WORD_MAX_LEN, f) != NULL) {
            entries[i].data = (uint8_t *)str_buffer;
            entries[i].n = strlen(str_buffer);
        }
    }
    fclose(f);
    // This is an array of function pointers
    // Which will let us iterate through all the different hash functions
    // The syntax is actually only slightly different from a normal function
    // for a function pointer, the name of the function is in parenthesis
    // and it has an asterisk in front of it. The brackets after the name
    // show it is an array of function pointers. Everything outside those parentheses
    // work like normal for the function definition, and the arguments don't need names.
    uint32_t (*hash_functions[])(uint8_t *, int) = {add_hash, table_a_hash, table_b_hash,
                                                    djb2a_hash, fnv1a_hash, fxhash32_hash};
    // This is a good idiom to get the number of elements in a static array in C
    // This only works because the hash function array is declared as a literal array with { ... }
    int n_hash_functions = sizeof(hash_functions) / sizeof(hash_functions[0]);

    uint32_t (*reduce_functions[])(uint32_t) = {modulo2_reduce, modulo_prime_reduce,
                                                fibonacci32_reduce};
    int n_reduce_functions = sizeof(reduce_functions) / sizeof(reduce_functions[0]);

    setup_table_hash();

    for (int hash_i = 0; hash_i < n_hash_functions; hash_i++) {
        for (int reduce_i = 0; reduce_i < n_reduce_functions; reduce_i++) {
            int fast_mode = 1;
            evaluate_hash_reduce(n_entries, entries, fast_mode, hash_functions[hash_i],
                                 reduce_functions[reduce_i]);
        }
        printf("\n");
    }
    for (int i = 0; i < n_entries; i++) {
        free(entries[i].data);
    }
    free(entries);
    return 0;
}
