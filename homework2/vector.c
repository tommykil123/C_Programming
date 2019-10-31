#include "vector.h"
#include <stdio.h>
#include <stdlib.h>

void init(vector_t *arr) {
    double **buff = malloc(NROWS * sizeof(*buff) + (NROWS * (NCOLS * sizeof(**buff))));
    size_t size = NROWS * sizeof(*buff) + (NROWS * (NCOLS * sizeof(**buff)));
    double *ptr = (double *)(buff + NROWS);
    for (int i = 0; i < NROWS; i++) {
        buff[i] = ptr + NCOLS * i;
    }
    arr->capacity = NROWS;
    arr->size = 0;
    arr->buffer = buff;
}

void append(vector_t *arr, double x, double y) {
    /*if (arr->size > arr->capacity - 1) {
        //printf("Expanding the vector!\n");
        arr->capacity *= 2;
        float **buff = arr->buffer;
        size_t size = NROWS * sizeof(*buff) + (NROWS * (NCOLS * sizeof(**buff)));
        //size_t size_new = NROWS * sizeof(arr->buffer) + (NROWS * (NCOLS * sizeof(*arr->buffer)));
        //printf("size: %ld, size_n: %ld\n", size, size_new);
        arr->buffer = realloc(arr->buffer, size * (size_t)arr->capacity);
        float *ptr = (float *)(arr->buffer + arr->capacity);
        for (int i = 0; i < arr->capacity; i++) {
            arr->buffer[i] = ptr + NCOLS * i;
        }
    }*/
    size_t arr_size = arr->size;
    arr->buffer[arr_size][0] = x;
    arr->buffer[arr_size][1] = y;
    arr->size += 1;
    //printf("x_a: %d, y_a:%d\n", arr->buffer[arr_size][0], arr->buffer[arr_size][1]);
}
