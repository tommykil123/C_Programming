#include <stdio.h>
#include <string.h>
int main(int argc, char **argv) {
    if (argc < 2) {
        printf("usage: ./upper <text>\n");
        return 1;
    }

    for (int i = 1; i < argc; i++) {
        int len_word = strlen(argv[i]);
        //char word[len_word];
        //strcpy(word, argv[i]);
        for (int j = 0; j < len_word; j++) {
            char letter = argv[i][j];
            if (letter >= 'a' && letter <= 'z') {
                letter = letter - 32;
            }
            printf("%c", letter);
        }
    }
    printf("\n");
    return 0;
}
