#include "hashtable.h"

typedef struct hashtable_entry_t {
    char *key;
    int value;
} hashtable_entry_t;

typedef struct hashtable_info_t {
    int num_entries;
    int collisions;
} hashtable_info_t;

typedef struct hashtable {
    hashtable_entry_t *entries;
    size_t table_size;
    hashtable_info_t info;
} hashtable_t;

int nth_power_of_two(int number) {
    int n = 0;
    while (number != 1) {
        number *= 0.5;
        n += 1;
    }
    return n;
}

void hashtable_destroy(hashtable_t *ht) {
    for (int i = 0; i < (int)ht->table_size; i++) {
        free(ht->entries[i].key);
    }
    free(ht->entries);
    free(ht);
}

hashtable_t *hashtable_create(int entry_size) {
    // size_t entry_size = 128;
    hashtable_t *table = calloc(1, sizeof(hashtable_t));
    hashtable_entry_t *entries = calloc(entry_size, sizeof(hashtable_entry_t));
    table->entries = entries;
    table->table_size = entry_size;
    table->info.collisions = 0;
    return table;
}

void print_table(hashtable_t *ht) {
    for (int i = 0; i < (int)ht->table_size; i++) {
        if (ht->entries[i].value != 0) {
            printf("index: %d, key: %s, value: %d\n", i, ht->entries[i].key, ht->entries[i].value);
        }
    }
}

int count_collisions(hashtable_t *ht) {
    int collisions = 0;
    int table_size = ht->table_size;
    uint16_t *table = calloc(table_size, sizeof(uint16_t));
    for (int i = 0; i < table_size; i++) {
        if (ht->entries[i].value > 0) {
            char *key = ht->entries[i].key;
            uint32_t hash = fxhash32_hash((uint8_t *)key, strlen(key));
            hash = fibonacci32_reduce(hash, nth_power_of_two(hashtable_probe_max(ht)));
            if (table[hash] == 0) {
                table[hash] = 1;
            } else {
                collisions += 1;
            }
        }
    }
    free(table);
    return collisions;
}

void rehash(hashtable_t *ht, char *key, int value) {
    // 27 to 7 --> 14 to 6
    int expanded_size = ht->table_size * 2;
    hashtable_t *ht_new = hashtable_create(expanded_size);
    for (int i = 0; i < (int)ht->table_size; i++) {
        if (ht->entries[i].value != 0) {
            int old_value = 0;
            hashtable_get(ht, ht->entries[i].key, &old_value);
            hashtable_set(ht_new, ht->entries[i].key, old_value - 1);
        }
    }
    hashtable_set(ht_new, key, value);
    int collision_cnt_old = count_collisions(ht);
    int collision_cnt_new = count_collisions(ht_new);
    printf("Rehashing reduced collisions from %d to %d\n",
           collision_cnt_old, collision_cnt_new);
    free(ht->entries);
    ht->entries = ht_new->entries;
    ht->info.num_entries = ht_new->info.num_entries;
    ht->table_size = expanded_size;
    ht->info.collisions = 0;
    free(ht_new);
}

void hashtable_set(hashtable_t *ht, char *key, int value) {
    uint32_t hash = fxhash32_hash((uint8_t *)key, strlen(key));
    hash = fibonacci32_reduce(hash, nth_power_of_two(hashtable_probe_max(ht)));
    if (((double)ht->info.num_entries / (double)ht->table_size) < LOAD_FACTOR) {
        uint32_t idx = hash;
        while (1) {
            if (idx >= ht->table_size) {
                idx = 0;
            }
            if (ht->entries[idx].value == 0) {
                if (idx != hash && ht->entries[idx].value == 0) {
                    // ht->info.collisions += 1;
                }
                ht->entries[idx].key = key;
                ht->entries[idx].value = value + 1;
                ht->info.num_entries += 1;
                break;
            }
            if (!strcmp(ht->entries[idx].key, key)) {
                ht->entries[idx].value = value + 1;
                // if (idx != hash) {
                //     ht->info.collisions += 1;
                // }
                free(key);
                break;
            }
            idx += 1;
        }
    } else {
        rehash(ht, key, value);
    }
}

bool hashtable_get(hashtable_t *ht, char *key, int *value) {
    uint32_t hash = fxhash32_hash((uint8_t *)key, strlen(key));
    hash = fibonacci32_reduce(hash, nth_power_of_two(hashtable_probe_max(ht)));
    uint32_t idx = hash;
    while (1) {
        if (idx >= ht->table_size) {
            idx = 0;
        }
        if (ht->entries[idx].value == 0) {
            return 0;
        }
        if (!strcmp(ht->entries[idx].key, key)) {
            *value = ht->entries[idx].value;
            return 1;
        } else {
            idx += 1;
        }
    }
}

int hashtable_size(hashtable_t *ht) {
    return ht->info.num_entries;
}

int hashtable_probe_max(hashtable_t *ht) {
    return ht->table_size;
}

bool hashtable_probe(hashtable_t *ht, int i, char **key, int *val) {
    if (ht->entries[i].value > 0) {
        *key = ht->entries[i].key;
        *val = ht->entries[i].value;
        return 1;
    }
    return 0;
}
