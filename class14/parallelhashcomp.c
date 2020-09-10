#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#define _GNU_SOURCE
#include <time.h>
#include "hashfunction.h"
#include <string.h>
#include <math.h>

typedef struct test_entry {
    uint8_t *data;
    int n;
} test_entry_t;

typedef struct thread_info {
    int num;
    pthread_t thread;
    int n_entries;
    test_entry_t *entries;
    uint32_t (*hash_f)(uint8_t *, int);
    uint32_t (*reduce_f)(uint32_t);
    int collision_num;
    double time_per_iter;
} thread_info_t;

double seconds_now(void) {
    struct timespec now;
    if (clock_gettime(CLOCK_MONOTONIC, &now)) {
        fprintf(stderr, "Retrieving system time failed.\n");
        exit(1);
    }
    return now.tv_sec + now.tv_nsec / 1000000000.0;
}

void evaluate_hash_reduce(int n_entries, test_entry_t *entries,
                          uint32_t (*hash_f)(uint8_t *, int), uint32_t (*reduce_f)(uint32_t),
                          int *collision_num, double *time_per_iter) {
    double start = seconds_now();
    int collisions = 0;
    int run_num = 0;
    while ((seconds_now() - start) < 0.5) {
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
    double time_sec = ((seconds_now() - start));
    double per_iter = (time_sec / (run_num * n_entries)) * pow(10, 9);
    *collision_num = collisions / run_num;
    *time_per_iter = per_iter;
}

void *thread_start(void *user) {
    thread_info_t *info = user;
    int n_entries = info->n_entries;
    test_entry_t *entries = info->entries;
    uint32_t (*hash_function)(uint8_t *, int) = info->hash_f;
    uint32_t (*reduce_function)(uint32_t) = info->reduce_f;
    int collision_num;
    double time_per_iter;
    evaluate_hash_reduce(n_entries, entries, hash_function, reduce_function,
                         &collision_num, &time_per_iter);
    info->collision_num = collision_num;
    info->time_per_iter = time_per_iter;
    return NULL;
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "usage: %s <parallelhashcomp> <N_THREADS>\n", argv[0]);
        return 1;
    }
    int N_THREADS = atoi(argv[1]);
    int max_entries = TABLE_SIZE / 2;
    int n_entries = 4096;
    test_entry_t *entries = calloc(max_entries, sizeof(test_entry_t));
    for (uint16_t i = 0; i < 1000; i++) {
        uint8_t *data = malloc(sizeof(i));
        memcpy(data, &i, sizeof(i));
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

    uint32_t (*hash_functions[])(uint8_t *, int) = {add_hash, table_a_hash, table_b_hash,
                                                    djb2a_hash, fnv1a_hash, fxhash32_hash};
    uint32_t (*reduce_functions[])(uint32_t) = {modulo2_reduce, modulo_prime_reduce,
                                                fibonacci32_reduce};
    int n_reduce_functions = sizeof(reduce_functions) / sizeof(reduce_functions[0]);
    int n_hash_functions = sizeof(hash_functions) / sizeof(hash_functions[0]);
    setup_table_hash();
    // =======================================================================================
    thread_info_t thread_infos[18];
    int thread_idx = 0;
    int thread_iter = 0;
    for (int hash_i = 0; hash_i < n_hash_functions; hash_i++) {
        for (int reduce_i = 0; reduce_i < n_reduce_functions; reduce_i++) {
            thread_infos[thread_idx].num = thread_idx;
            thread_infos[thread_idx].n_entries = n_entries;
            thread_infos[thread_idx].entries = entries;
            thread_infos[thread_idx].hash_f = *hash_functions[hash_i];
            thread_infos[thread_idx].reduce_f = *reduce_functions[reduce_i];
            pthread_create(&thread_infos[thread_idx].thread, NULL, thread_start,
                           &thread_infos[thread_idx]);
            if ((thread_idx + 1) % N_THREADS == 0) {
                for (int i = 0; i < N_THREADS; i++) {
                    pthread_join(thread_infos[thread_iter * N_THREADS + i].thread, NULL);
                }
                thread_iter += 1;
            } else if ((hash_i == n_hash_functions - 1 && reduce_i == n_reduce_functions - 1) &&
                       ((n_hash_functions * n_reduce_functions) % N_THREADS) != 0) {
                for (int i = 0; i < (n_hash_functions * n_reduce_functions) % N_THREADS; i++) {
                    pthread_join(thread_infos[thread_iter * N_THREADS + i].thread, NULL);
                }
                break;
            }
            thread_idx += 1;
        }
    }
    for (int i = 0; i < n_hash_functions * n_reduce_functions; i++) {
        printf("%.2fns per iteration, with %d collisions\n",
               thread_infos[i].time_per_iter, thread_infos[i].collision_num);
        if (((i + 1) % 3) == 0) {
            printf("\n");
        }
    }
    // =======================================================================================
    for (int i = 0; i < n_entries; i++) {
        free(entries[i].data);
    }
    free(entries);
    return 0;
}
