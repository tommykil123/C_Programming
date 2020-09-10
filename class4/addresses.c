#include <stdio.h>
#include <stdlib.h>
char char1;
int int1;
double doubles[3];
int main(void) {
    printf("%p\n", (void *)&char1);
    printf("%p\n", (void *)&int1);
    printf("%ld\n", (long)&char1 - (long)&int1);
    printf("%p\n", (void *)&doubles);
    printf("%p\n", (void *)&doubles[0]);
    printf("%p\n", (void *)&doubles[1]);
    printf("%p\n", (void *)&doubles[2]);
    printf("%ld\n", (void *)&doubles[0] - (void *)&doubles[1]);
    printf("%ld\n", (long)&doubles[0] - (long)&doubles[1]);

    char char2;
    int int2;
    float floats[3];
    printf("%p\n", (void *)&char2);
    printf("%p\n", (void *)&int2);
    printf("%ld\n", (long)&char2 - (long)&int2);
    printf("%p\n", (void *)&floats);
    printf("%p\n", (void *)&floats[0]);
    printf("%p\n", (void *)&floats[1]);
    printf("%p\n", (void *)&floats[2]);
    printf("%ld\n", (void *)&floats[0] - (void *)&floats[1]);
    printf("%ld\n", (long)&floats[0] - (long)&floats[1]);

    void *mem = malloc(1); // ask for memory
    printf("%p\n", mem);   // memory is already "given" to us as an address!
    free(mem);             // give memory back
    return 0;
}
