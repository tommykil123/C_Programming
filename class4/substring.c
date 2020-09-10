#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char file_buffer[64 * 1024];

char *substring(char *file, char *key) {
    return strstr(file, key);
}

int main(int argc, char **argv) {
    if (argc != 4) {
        fprintf(stderr, "usage: %s <file> <key> <lines before>\n", argv[0]);
        return 1;
    }

    FILE *f = fopen(argv[1], "r");
    if (!f) {
        fprintf(stderr, "Could not open %s: ", argv[1]);
        perror("");
        return 1;
    }
    int bytes_read = fread(file_buffer, 1, sizeof(file_buffer) - 1, f);
    if (!feof(f)) {
        fprintf(stderr, "Could not read entire file. Is it too big?\n");
        return 1;
    }
    fclose(f);
    // we want this to be a null-treminated string,
    // but fread just reads the file as binary, so we add it ourselves
    file_buffer[bytes_read] = '\0';
    char *current_pt = substring(file_buffer, argv[2]);
    while (current_pt) {
        int idx4begin = 0;
        int linesread = 0;
        while (1) {
            char *line_begin_pt = current_pt - idx4begin;
            if (atoi(argv[3]) == 0) {
                idx4begin = 1;
                break;
            }
            if (line_begin_pt[0] == '\n') {
                linesread += 1;
                if (linesread == atoi(argv[3])) {
                    break;
                }
            }
            if (line_begin_pt[0] == 0) {
                break;
            }
            idx4begin += 1;
        }
        char *tmp = current_pt + strlen(argv[2]);
        char character = tmp[0];
        tmp[0] = 0;
        printf("%s\n\n", current_pt - idx4begin + 1);
        tmp[0] = character;
        // Going to the next instance of the word
        current_pt = substring(current_pt + 1, argv[2]);
    }
    return 0;
}
