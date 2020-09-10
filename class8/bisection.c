#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define EPSILON 1e-6

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "usage: %s <low> <high>\n", argv[0]);
        exit(1);
    }
    int poly[5] = {0, 2, -20, 20, -4};
    double x_low = atof(argv[1]);
    double x_high = atof(argv[2]);
    double diff = x_high - x_low;
    while (diff > EPSILON) {
        double x_mid = 0;
        double y_low = 0;
        double y_mid = 0;
        double y_high = 0;
        x_mid = (x_high + x_low) / 2;
        for (int i = 0; i < sizeof(poly) / sizeof(int); i++) {
            y_low += poly[i] * pow(x_low, i);
            y_mid += poly[i] * pow(x_mid, i);
            y_high += poly[i] * pow(x_high, i);
        }
        if (y_low * y_mid > 0) {
            x_low = x_mid;
        } else {
            x_high = x_mid;
        }
        diff = x_high - x_low;
        printf("x_mid: %11.4e y_mid: %11.4e\n", x_mid, y_mid);
    }
    return 0;
}
