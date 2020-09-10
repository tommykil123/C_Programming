#pragma once
#include <stdint.h>
#define FNV_offset_basis 2166136261
#define FNV_prime 16777619
#define TABLE_SIZE 8192
#define WORD_MAX_LEN 256

uint32_t table_hash_keys[256];

uint32_t add_hash(uint8_t *data, int n);
void setup_table_hash(void);
uint32_t table_a_hash(uint8_t *data, int n);
uint32_t table_b_hash(uint8_t *data, int n);
uint32_t djb2a_hash(uint8_t *data, int n);
uint32_t fnv1a_hash(uint8_t *data, int n);
uint32_t rotate_left(uint32_t value, uint32_t count);
uint32_t fxhash32_step(uint32_t hash, uint32_t value);
int remaining_bytes(uint8_t *data);
uint32_t fxhash32_hash(uint8_t *data, int n);
uint32_t modulo2_reduce(uint32_t hash);
uint32_t modulo_prime_reduce(uint32_t hash);
uint32_t fibonacci32_reduce(uint32_t hash, int hash_length);
