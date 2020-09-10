#include <stdlib.h>
#include <stdio.h>

typedef struct vector {
    int *buffer;
    int capacity; // size of buffer
    int size;     // size of vector
} vector_t;

void init(vector_t *arr) {
    int *buff = malloc(sizeof(*buff) * 3);
    buff[0] = 1;
    buff[1] = 2;
    buff[2] = 2;
    arr->capacity = 3;
    arr->size = 3;
    arr->buffer = buff;
}

void append(vector_t *arr, int x) {
    if (arr->size > arr->capacity - 1) {
        arr->capacity *= 2;
        arr->buffer = realloc(arr->buffer, sizeof(*arr->buffer) * arr->capacity);
    }
    arr->buffer[arr->size] = x;
    arr->size += 1;
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "usage: %s <max golomb number>\n", argv[0]);
    }
    if (atoi(argv[1]) == 0) {
        return 0;
    }

    vector_t arr;
    init(&arr);
    for (int i = 3; i < atoi(argv[1]) + 1; i++) {
        for (int j = 0; j < arr.buffer[i - 1]; j++) {
            append(&arr, i);
        }
    }

    if (atoi(argv[1]) == 1) {
        printf("%d\n", arr.buffer[0]);
        free(arr.buffer);
        return 0;
    }
    for (int i = arr.size - 1; i >= 0; i--) {
        printf("%d\n", arr.buffer[i]);
    }
    free(arr.buffer);
    return 0;
}
