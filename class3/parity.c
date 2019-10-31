#include <stdio.h>
#include <string.h>
int main(int argc, char **argv) {
    if (argc > 2) {
        printf("usage: ./parity [<text>]\n");
        return 1;
    }

    if (argc == 1) {
        printf("%d\n", 0);
    } else {
        int b_odd_ones = 0; // is there an odd number of 1s
        for (int i = 0; i < strlen(argv[1]); i++) {
            char letter = argv[1][i];
            char tmp = letter ^ (letter >> 1);
            tmp = tmp ^ (tmp >> 2);
            tmp = tmp ^ (tmp >> 4);
            int b_tmp = 0;
            if (tmp & 1) {
                b_tmp = 1;
            } else {
                b_tmp = 0;
            }
            b_odd_ones = b_tmp ^ b_odd_ones;
        }

        if (b_odd_ones & 1) {
            printf("%d\n", 1);
        } else {
            printf("%d\n", 0);
        }
    }
    return 0;
}
