#pragma once

#define NROWS 5000
#define NCOLS 2

typedef struct vector {
    double **buffer;
    int capacity; // size of buffer
    int size; // size of vector
} vector_t;

void init(vector_t *arr);
void append(vector_t *arr, double x, double y);
