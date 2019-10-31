#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

char *trit_encodings[243];

int main(void) {
    for (int i = 0; i < 243; i++) {
        trit_encodings[i] = calloc(sizeof(char), 6);
    }
    for (int i = 0; i < 243; i++) {
        int mod3 = i % 3;
        if (mod3 == 0) {
            trit_encodings[i][0] = ' ';
        } else if (mod3 == 1) {
            trit_encodings[i][0] = ':';
        } else {
            trit_encodings[i][0] = '@';
        }

        int mod9 = i % 9;
        if (mod9 >= 0 && mod9 < 3) {
            trit_encodings[i][1] = ' ';
        } else if (mod9 >= 3 && mod9 < 6) {
            trit_encodings[i][1] = ':';
        } else {
            trit_encodings[i][1] = '@';
        }

        int mod27 = i % 27;
        if (mod27 >= 0 && mod27 < 9) {
            trit_encodings[i][2] = ' ';
        } else if (mod27 >= 9 && mod27 < 18) {
            trit_encodings[i][2] = ':';
        } else {
            trit_encodings[i][2] = '@';
        }

        int mod81 = i % 81;
        if (mod81 >= 0 && mod81 < 27) {
            trit_encodings[i][3] = ' ';
        } else if (mod81 >= 27 && mod81 < 54) {
            trit_encodings[i][3] = ':';
        } else {
            trit_encodings[i][3] = '@';
        }

        int mod243 = i % 243;
        if (mod243 >= 0 && mod243 < 81) {
            trit_encodings[i][4] = ' ';
        } else if (mod243 >= 81 && mod243 < 162) {
            trit_encodings[i][4] = ':';
        } else {
            trit_encodings[i][4] = '@';
        }
    }

    FILE *fp = fopen("img.bin", "rb");

    // Return if could not open file
    int count = 0;
    if (!fp) {
        return 0;
    }
    while (1) {
        char c = fgetc(fp); // Taking input single character at a time
        uint8_t idx = (uint8_t)(c);
        if (feof(fp)) { // Checking for end of file
            break;
        }
        if ((count + 1) % 18 == 0) {
            printf("%s\n", trit_encodings[idx]);
        } else {
            printf("%s", trit_encodings[idx]);
        }
        count += 1;
    }

    fclose(fp);
    return 0;
}
