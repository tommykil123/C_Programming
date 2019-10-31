#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct node {
    struct node *prev;
    struct node *next;
    int value;
} node_t;

typedef struct list {
    node_t *start;
    node_t *end;
} list_t;

list_t *list_create(void) {
    list_t *temp = malloc(sizeof(*temp));
    temp->start = NULL;
    temp->end = NULL;
    return temp;
}

void list_push_start(list_t *list, int value) {
    if (!list->start && !list->end) {
        node_t *begin = malloc(sizeof(*begin));
        begin->value = value;
        list->start = begin;
        list->end = list->start;
    } else {
        node_t *new_begin = malloc(sizeof(*new_begin));
        new_begin->value = value;
        new_begin->next = list->start;
        list->start->prev = new_begin;
        list->start = new_begin;
    }
}

void list_push_end(list_t *list, int value) {
    if (!list->start && !list->end) {
        node_t *end = malloc(sizeof(*end));
        end->value = value;
        list->end = end;
        list->start = list->end;
    } else {
        node_t *new_end = malloc(sizeof(*new_end));
        new_end->value = value;
        new_end->prev = list->end;
        list->end->next = new_end;
        list->end = new_end;
    }
}

int list_pop_start(list_t *list) {
    int value = 0;
    if (list->start) {
        value = list->start->value;
        node_t *temp = list->start;
        if (list->start != list->end) {
            list->start = list->start->next;
        } else {
            list->start = NULL;
            list->end = NULL;
        }
        free(temp);
    } else {
        fprintf(stderr, "can't pop from an empty list!\n");
        exit(1);
    }
    return value;
}

int list_pop_end(list_t *list) {
    int value = 0;
    if (list->end) {
        value = list->end->value;
        node_t *temp = list->end;
        if (list->start != list->end) {
            list->end = list->end->prev;
        } else {
            list->start = NULL;
            list->end = NULL;
        }
        free(temp);
    } else {
        fprintf(stderr, "can't pop from an empty list!\n");
        exit(1);
    }
    return value;
}

void list_destroy(list_t *list) {
    while (list->start && list->end) {
        list_pop_end(list);
    }
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "usage: %s <file>\n", argv[0]);
        return 1;
    }

    FILE *f = fopen(argv[1], "r");
    if (!f) {
        fprintf(stderr, "could not open %s: ", argv[1]);
        perror("");
        return 1;
    }

    list_t *list = list_create();

    while (!feof(f)) {
        char command[16];
        int res = fscanf(f, "%10s", command);
        if (res != 1) {
            if (!feof(f)) {
                fprintf(stderr, "trouble parsing command. stopping.\n");
            }
            break;
        }

        if (strcmp("push_start", command) == 0) {
            int value = 0;
            res = fscanf(f, "%d", &value);
            if (res != 1) {
                fprintf(stderr, "trouble parsing value. stopping.\n");
                break;
            }

            list_push_start(list, value);
        } else if (strcmp("push_end", command) == 0) {
            int value = 0;
            res = fscanf(f, "%d", &value);
            if (res != 1) {
                fprintf(stderr, "trouble parsing value. stopping.\n");
                break;
            }

            list_push_end(list, value);
        } else if (strcmp("pop_start", command) == 0) {
            int v = list_pop_start(list);
            printf("%d\n", v);
        } else if (strcmp("pop_end", command) == 0) {
            int v = list_pop_end(list);
            printf("%d\n", v);
        } else {
            fprintf(stderr, "did not recognize command '%s'. stopping.\n", command);
            break;
        }
    }

    list_destroy(list);
    free(list);

    fclose(f);

    return 0;
}
