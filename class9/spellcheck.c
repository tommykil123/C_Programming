#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define WORD_MAX_LEN 256

// ternary search tree node
typedef struct tst_node {
    char c;
    // just in this structure definition we have to call the type
    // "struct tst_node" because the tst_node_t definition doesn't exist yet
    struct tst_node *low;
    struct tst_node *equal;
    struct tst_node *high;
} tst_node_t;

// ternary search tree
typedef struct tst {
    tst_node_t *node;
} tst_t;

tst_node_t *tst_node_create(char c) {
    tst_node_t *node = calloc(1, sizeof(*node)); // calloc also zeros out the memory
    node->c = c;
    return node;
}

void tst_node_destroy(tst_node_t *node) {
    if (!node) {
        return;
    }
    tst_node_destroy(node->low);
    tst_node_destroy(node->equal);
    tst_node_destroy(node->high);
    free(node);
}

tst_t *tst_create(void) {
    return calloc(1, sizeof(tst_t)); // calloc also zeros out the memory
}

void tst_destroy(tst_t *tst) {
    tst_node_destroy(tst->node);
    free(tst);
}

void tst_add(tst_t *tst, const char *word) {
    tst_node_t *node_track = tst->node;
    int i = 0;
    if (!tst->node) { // Initialize first node
        tst_node_t *first_node = tst_node_create(word[i]);
        tst->node = first_node;
        node_track = first_node;
    }
    while (1) {
        if (node_track->c == word[i]) {
            if (word[i] == '\0') {
                break;
            }
            if (!node_track->equal) {
                tst_node_t *equal_node = tst_node_create(word[i + 1]);
                node_track->equal = equal_node;
            }
            node_track = node_track->equal;
            i += 1;
        } else {
            if (node_track->c > word[i]) { // Character Less
                if (!node_track->low) {
                    tst_node_t *left_node = tst_node_create(word[i]);
                    node_track->low = left_node;
                }
                node_track = node_track->low;
            } else { // Character Greater
                if (!node_track->high) {
                    tst_node_t *right_node = tst_node_create(word[i]);
                    node_track->high = right_node;
                }
                node_track = node_track->high;
            }
        }
    }
}

void tst_node_search(tst_node_t *node, char *word, char *suggestion, char *sugg_start, int errs) {
    while (node) {
        if (errs > 0) {
            // insert
            tst_node_search(node, word + 1, suggestion, sugg_start, errs - 1);
            // tranpose
            char curr = word[0];
            char next = word[1];
            word[0] = next;
            word[1] = curr;
            tst_node_search(node, word, suggestion, sugg_start, errs - 1); // transpose
            word[0] = curr;
            word[1] = next;
            // replace
            if (node->c != '\0') {
                char orig = word[0];
                word[0] = node->c;
                tst_node_search(node, word, suggestion, sugg_start, errs - 1); // replace
                word[0] = orig;
            }
            // delete
            suggestion[0] = node->c;
            tst_node_search(node->equal, word, suggestion + 1, sugg_start, errs - 1); // delete
        }
        if (node->c > word[0]) {
            tst_node_search(node->high, word, suggestion, sugg_start, errs);
            node = node->low;
        } else if (node->c < word[0]) {
            tst_node_search(node->low, word, suggestion, sugg_start, errs);
            node = node->high;
        } else {
            tst_node_search(node->low, word, suggestion, sugg_start, errs);
            tst_node_search(node->high, word, suggestion, sugg_start, errs);
            suggestion[0] = node->c;
            if (node->c == '\0' && word[0] == '\0') {
                printf("%s\n", sugg_start);
            }
            node = node->equal;
            suggestion += 1;
            word += 1;
        }
    }
}

void tst_search(tst_t *tst, char *word) {
    if (!tst->node) {
        return;
    }
    char word_suggestion[WORD_MAX_LEN];
    tst_node_search(tst->node, word, &word_suggestion[0], word_suggestion, 1);
}

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "usage: %s <word>+\n", argv[0]);
        return 1;
    }

    FILE *f = fopen("words.txt", "r");
    // FILE *f = fopen("words_tmp.txt", "r");
    // FILE *f = fopen("words_hospital.txt", "r");
    // FILE *f = fopen("words_hospital_wth_others.txt", "r");
    if (!f) {
        perror("Failed to open words.txt");
        return 1;
    }

    tst_t *tst = tst_create();

    char str_buffer[WORD_MAX_LEN];
    while (1) {
        int ret = fscanf(f, "%255s", str_buffer);
        if (ret != 1) {
            break;
        }
        tst_add(tst, str_buffer);
    }
    fclose(f);

    for (int i = 1; i < argc; i++) {
        if (strlen(argv[i]) >= WORD_MAX_LEN) {
            fprintf(stderr, "word '%s' is too long!\n", argv[i]);
            continue;
        }

        tst_search(tst, argv[i]);
        printf("\n");
    }

    tst_destroy(tst);
    return 0;
}
