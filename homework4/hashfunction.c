#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include "hashfunction.h"

uint32_t add_hash(uint8_t *data, int n) {
    uint32_t hash = 0;
    for (int i = 0; i < n; i++) {
        hash += *data;
        data++;
    }
    return hash;
}

void setup_table_hash(void) {
    for (int i = 0; i < 256; i++) {
        table_hash_keys[i] = rand();
    }
}

uint32_t table_a_hash(uint8_t *data, int n) {
    uint32_t hash = 0;
    for (int i = 0; i < n; i++) {
        hash += table_hash_keys[data[0]];
        data += 1;
    }
    return hash;
}

uint32_t table_b_hash(uint8_t *data, int n) {
    uint32_t hash = 0;
    for (int i = 0; i < n; i++) {
        hash = (hash << 3) ^ table_hash_keys[data[0]];
        data += 1;
    }
    return hash;
}

uint32_t djb2a_hash(uint8_t *data, int n) {
    uint32_t hash = 5381;
    for (int i = 0; i < n; i++) {
        hash = ((hash << 5) + hash) ^ data[0];
        data += 1;
    }
    return hash;
}

uint32_t fnv1a_hash(uint8_t *data, int n) {
    uint32_t hash = FNV_offset_basis;
    for (int i = 0; i < n; i++) {
        hash = hash ^ data[0];
        hash = hash * FNV_prime;
        data += 1;
    }
    return hash;
}

uint32_t rotate_left(uint32_t value, uint32_t count) {
    return value << count | value >> (32 - count);
}

uint32_t fxhash32_step(uint32_t hash, uint32_t value) {
    const uint32_t key = 0x27220a95;
    // const uint64_t key = 0x517cc1b727220a95;
    return (rotate_left(hash, 5) ^ value) * key;
}

int remaining_bytes(uint8_t *data) {
    int i = 0;
    while (data[i] != '\0') {
        i += 1;
    }
    return i;
}

uint32_t fxhash32_hash(uint8_t *data, int n) {
    uint32_t hash = 0;
    // while (remaining_bytes(data) >= 4) {
    int iter_4byte = n / 4;
    for (int i = 0; i < iter_4byte; i++) {
        uint32_t number;
        memcpy(&number, data, sizeof(number));
        hash = fxhash32_step(hash, number);
        data += 4;
    }
    for (int i = 0; i < (n - iter_4byte * 4); i++) {
        hash = fxhash32_step(hash, data[0]);
        data += 1;
    }
    return hash;
}

uint32_t modulo2_reduce(uint32_t hash) {
    hash = hash & (TABLE_SIZE - 1);
    return hash;
}

uint32_t modulo_prime_reduce(uint32_t hash) {
    hash = hash % 8191;
    return hash;
}

uint32_t fibonacci32_reduce(uint32_t hash, int hash_length) {
    double phi = (1 + sqrt(5)) / 2;
    uint32_t factor32 = pow(2, 32) / phi;
    hash = (hash * factor32) >> (32 - hash_length);
    return hash;
}
