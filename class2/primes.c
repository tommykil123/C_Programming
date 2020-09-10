#include <stdio.h>
#include <stdlib.h>
#include <math.h>
int main(void) {
    printf("Enter a number to factorize:\n");
    int number = 0;
    int vals_read = scanf("%d", &number);
    if (vals_read != 1) {
        fprintf(stderr, "Invalid input. Please enter a number.\n");
        return 1;
    }

    int idx = 0;
    int max_size = floor(sqrt(number));
    // int prime_array[max_size];
    // int prime_found = 0;

    printf("Prime factors:\n");

    while (number % 2 == 0) {
        number = number / 2;
        //prime_found = 1;
        printf("%d\n", 2);
    }
    /*if (prime_found == 1) {
        prime_array[idx] = 2;
        idx += 1;
    }*/

    for (int i = 3; i <= max_size; ++i) {
        //prime_found = 0;
        while (number % i == 0) {
            number = number / i;
            //prime_found = 1;
            printf("%d\n", i);
        }
        /*if (prime_found == 1) {
            prime_array[idx] = i;
            idx += 1;
        }*/
    }

    if (number != 1) {
        printf("%d\n", number);
    }
    /*for (int i = 0; i < idx; i++) {
        printf("%d\n", prime_array[i]);
    }*/
    return 0;
}
