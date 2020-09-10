#pragma once
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "hashfunction.h"
#define LOAD_FACTOR 0.5

typedef struct hashtable hashtable_t;

int nth_power_of_two(int number);
hashtable_t *hashtable_create(int entry_size);
void hashtable_destroy(hashtable_t *ht);
void rehash(hashtable_t *ht, char *key, int value);
void print_table(hashtable_t *ht);
int count_collisions(hashtable_t *ht);
// Inserts or modifies the value associated with key
void hashtable_set(hashtable_t *ht, char *key, int value);

// Returns true if key is present in ht and if so, fills *value with the value from ht
// otherwise returns false and does not modify *value
bool hashtable_get(hashtable_t *ht, char *key, int *value);

// The number of entries (distinct keys) in ht
int hashtable_size(hashtable_t *ht);

// Use this alongside hashtable_probe
// to iterate through the table
int hashtable_probe_max(hashtable_t *ht);

// permits iterating through the table
// iterate with i from 0 to hashtable_probe_max
// and if this function returns true, key and val are copied to.
// Do not mutate key!
bool hashtable_probe(hashtable_t *ht, int i, char **key, int *val);
