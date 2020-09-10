#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define ARR_SIZE 1000

int int_compare(const void *a, const void *b) {
    int val_a = *(int *)a;
    int val_b = *(int *)b;
    return val_a - val_b;
}

int binary_search_leftmost(const int *arr, int arr_size, int val) {
    int L_idx = 0;
    int R_idx = arr_size;
    int mid_idx;
    while (L_idx < R_idx) {
        mid_idx = floorf(((float)L_idx + (float)R_idx) / 2);
        printf("mid_i: %d\n", mid_idx);
        if (arr[mid_idx] < val) {
            L_idx = mid_idx + 1;
        } else {
            R_idx = mid_idx;
        }
    }
    if (arr[L_idx] != val) {
        L_idx = -1;
    }
    return L_idx;
}

int main(int argc, char **argv) {
    /*if (argc != 2) {
        //fprintf(stderr, "usage: %s <number>\n", argv[0]);
        return 1;
    }*/
    int value = atoi(argv[1]);
    int *arr = malloc(sizeof(*arr) * ARR_SIZE);
    for (int i = 0; i < ARR_SIZE; i++) {
        arr[i] = rand() % ARR_SIZE;
    }
    qsort(arr, ARR_SIZE, sizeof(int), int_compare);
    int idx = binary_search_leftmost(arr, ARR_SIZE, value);
    if (idx != -1) {
        printf("%d\n", idx);
    }
    free(arr);
    return 0;
}
