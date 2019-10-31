#include <stdio.h>
#include <string.h>
#include <stdint.h>

int main(int argc, char **argv) {
    if (argc == 1) {
        fprintf(stderr, "usage: ./crytogram <encrypt|decrypt> <password> <text>\n");
        return 1;
    }
    if (!(strcmp(argv[1], "encrypt") == 0 || strcmp(argv[1], "decrypt") == 0)) {
        fprintf(stderr, "method given is neither encrypt nor decrypt\n");
        return 1;
    }

    int pass_length = strlen(argv[2]);
    int text_sizeof = strlen(argv[3]);
    int pass_count = 0;

    for (int i = 0; i < text_sizeof; i++) {
        if (pass_length == 0) {
            printf("%c", argv[3][i]);
            continue;
        }
        int letter_rot = 0;
        int pass_idx = pass_count % pass_length;
        char pass_letter = argv[2][pass_idx];
        while (1) {
            if (pass_letter < 65 || (pass_letter > 90 && pass_letter < 97) || pass_letter > 122) {
                pass_count += 1;
                pass_idx = pass_count % pass_length;
                pass_letter = argv[2][pass_idx];
                continue;
            }
            break;
        }
        if (argv[3][i] < 65 || (argv[3][i] > 90 && argv[3][i] < 97) || argv[3][i] > 122) {
            printf("%c", argv[3][i]);
            continue;
        }
        if (pass_letter >= 97) {
            letter_rot = pass_letter - 97;
        } else {
            letter_rot = pass_letter - 65;
        }

        int upper_case = 0;
        if (argv[3][i] < 91) {
            upper_case = 1;
        }

        uint8_t new_letter;
        if (strcmp(argv[1], "encrypt") == 0) {
            if (upper_case) {
                new_letter = 65 + (argv[3][i] - 65 + letter_rot) % 26;
            } else {
                new_letter = 97 + (argv[3][i] - 97 + letter_rot) % 26;
            }
        } else {
            if (upper_case) {
                new_letter = 90 - (90 - argv[3][i] + letter_rot) % 26;
            } else {
                new_letter = 122 - (122 - argv[3][i] + letter_rot) % 26;
            }
            // new_letter = argv[3][i] - letter_rot;
        }
        printf("%c", new_letter);
        pass_count += 1;
    }
    printf("\n");
    return 0;
}
