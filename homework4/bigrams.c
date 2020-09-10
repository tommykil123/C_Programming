#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "hashtable.h"

int read_word(FILE *f, char *word) {
    int i = 0;
    bool first_char = false;
    while ((!feof(f))) {
        char c = fgetc(f);
        if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) {
            if (!first_char) {
                first_char = true;
            }
            word[i] = c;
            i += 1;
        } else if (first_char) {
            break;
        }
    }
    return i;
}

int main(int argc, char **argv) {
    if (argc != 1) {
        fprintf(stderr, "usage: %s\n", argv[0]);
        return 1;
    }
    int init_size = 128;
    // int init_size = 4;
    hashtable_t *ht = hashtable_create(init_size);
    // ========= Open the text for reading ========= [S]
    // FILE *f = fopen("book_short.txt", "r");
    // FILE *f = fopen("book_temp.txt", "r");
    FILE *f = fopen("book.txt", "r");
    if (!f) {
        perror("Failed to open book.txt");
        return 1;
    }
    // ========= Open the text for reading ========= [E]
    // -------------------------------------------------
    // ============= Reading the text ============== [S]
    bool b_first_word = false;
    int length_w2;
    char word1[256] = {0};
    while ((!feof(f))) {
        char word2[256] = {0};
        if (!b_first_word) {
            read_word(f, word1);
            length_w2 = read_word(f, word2);
            b_first_word = true;
        } else {
            length_w2 = read_word(f, word2);
        }
        if (length_w2) {
            char *bi_word = malloc(256);
            strcat(word1, " ");
            strcat(word1, word2);
            strcpy(bi_word, word1);
            strcpy(word1, word2);
            char bi_word_copy[256] = "\0";
            strcpy(bi_word_copy, bi_word);
            int value = 0;
            hashtable_get(ht, bi_word_copy, &value);
            hashtable_set(ht, bi_word, value);
            // printf("Adding to hash %s\n", bi_word_copy);
            // print_table(ht);
            // printf("\n");
        } else {
            break;
        }
    }
    // ============= Reading the text ============== [E]
    // =========== Iterate through table =========== [S]
    bool found_200 = false;
    int n = hashtable_probe_max(ht);
    for (int i = 0; i < n; i++) {
        char *key;
        int val;
        if (hashtable_probe(ht, i, &key, &val)) {
            if (val >= 200) {
                found_200 = true;
                printf("Bigram '%s' has count of %d\n", key, val);
            }
        }
    }
    if (!found_200) {
        for (int i = 0; i < n; i++) {
            char *key;
            int val;
            if (hashtable_probe(ht, i, &key, &val)) {
                printf("Bigram '%s' has count of %d\n", key, val);
            }
        }
    }
    printf("Total of %d different bigrams recorded\n", hashtable_size(ht));
    hashtable_destroy(ht);
    // =========== Iterate through table =========== [E]
    fclose(f);
    return 0;
}
