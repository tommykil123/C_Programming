#pragma once

// #define NROWS 1720
// #define NCOLS 2

typedef struct vector {
    double *buffer_x;
    double *buffer_y;
    int capacity; // size of buffer
    int size; // size of vector
} vector_t;

void init(vector_t *arr);
void append(vector_t *arr, double x, double y);
