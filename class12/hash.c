#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define FNV_offset_basis 2166136261
#define FNV_prime 16777619

uint32_t table_hash_keys[256];

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
    for (int i = 0; i < (n / 4); i++) {
        uint32_t number;
        memcpy(&number, data, sizeof(number));
        hash = fxhash32_step(hash, number);
        data += 4;
    }
    while (data[0] != '\0') {
        hash = fxhash32_step(hash, data[0]);
        data += 1;
    }
    return hash;
}

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "usage: %s <hash func> <key>\n", argv[0]);
        return 1;
    }
    char *hash_func = argv[1];
    uint8_t *key = (uint8_t *)argv[2];
    int key_length = strlen(argv[2]);

    uint32_t hash = 0;
    if (!strcmp(hash_func, "add")) {
        hash = add_hash(key, key_length);
    } else if (!strcmp(hash_func, "table_a")) {
        setup_table_hash();
        hash = table_a_hash(key, key_length);
    } else if (!strcmp(hash_func, "table_b")) {
        setup_table_hash();
        hash = table_b_hash(key, key_length);
    } else if (!strcmp(hash_func, "djb2a")) {
        hash = djb2a_hash(key, key_length);
    } else if (!strcmp(hash_func, "fnv1a")) {
        hash = fnv1a_hash(key, key_length);
    } else if (!strcmp(hash_func, "fxhash32")) {
        hash = fxhash32_hash(key, key_length);
    } else {
        fprintf(stderr, "incorrect hash function specified ... aborting\n");
        return 1;
    }
    printf("0x%x\n", hash);
    return 0;
}
