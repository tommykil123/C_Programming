#include "vector.h"
#include <stdio.h>
#include <stdlib.h>

void init(vector_t *arr) {
    arr->capacity = 10;
    double *buff_x = malloc(sizeof(*buff_x) * arr->capacity);
    double *buff_y = malloc(sizeof(*buff_y) * arr->capacity);
    arr->size = 0;
    arr->buffer_x = buff_x;
    arr->buffer_y = buff_y;
}

void append(vector_t *arr, double x, double y) {
    if (arr->size > arr->capacity - 1) {
        arr->capacity *= 2;
        arr->buffer_x = realloc(arr->buffer_x, sizeof(*arr->buffer_x) * arr->capacity);
        arr->buffer_y = realloc(arr->buffer_y, sizeof(*arr->buffer_y) * arr->capacity);
    }
    arr->buffer_x[arr->size] = x;
    arr->buffer_y[arr->size] = y;
    arr->size += 1;
}
