#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "bmp.h"
#include "image_server.h"
#include "graphics.h"

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "usage: %s <test case>\n", argv[0]);
        exit(1);
    }
    color_bgr_t white = {255, 255, 255};
    bitmap_t bmp = {0};
    bmp.width = 640;
    bmp.height = 480;
    bmp.data = calloc(bmp.width * bmp.height, sizeof(color_bgr_t));
    vector_t poly_points;
    init(&poly_points);
    vector_t poly_graphic_points;
    init(&poly_graphic_points);

    if (atoi(argv[1]) == 0) {
        fprintf(stderr, "usage: %s <test case>\n", argv[0]);
    } else if (atoi(argv[1]) == 1) {
    } else if (atoi(argv[1]) == 2) {
        plotLine(&poly_points, 10, 10, 200, 200);
        plotLine(&poly_graphic_points, 10, 10, 200, 200);
    } else if (atoi(argv[1]) == 3) {
        drawRectangle(&poly_points, 4, 4);
        roundPoints(poly_points, &poly_graphic_points);
    } else if (atoi(argv[1]) == 4) {
        drawRectangle(&poly_points, 4, 4);
        translate(&poly_points, 2, 2);
        roundPoints(poly_points, &poly_graphic_points);
    } else if (atoi(argv[1]) == 5) {
        drawRectangle(&poly_points, 5, 5);
        translate(&poly_points, 2, 2);
        roundPoints(poly_points, &poly_graphic_points);
    } else if (atoi(argv[1]) == 6) {
        drawRectangle(&poly_points, 600, 440);
        translate(&poly_points, 640.0 / 2.0, 480.0 / 2.0);
        roundPoints(poly_points, &poly_graphic_points);
    } else if (atoi(argv[1]) == 7) {
        drawRectangle(&poly_points, 600, 440);
        translate(&poly_points, 640.0 / 2.0, 480.0 / 2.0);
        roundPoints(poly_points, &poly_graphic_points);
    } else if (atoi(argv[1]) == 8) {
        drawTriangle(&poly_points, 28);
        translate(&poly_points, 400, 400);
        roundPoints(poly_points, &poly_graphic_points);
    } else if (atoi(argv[1]) == 9) {
        drawTriangle(&poly_points, 28);
        rotate(&poly_points, 30.0);
        translate(&poly_points, 400, 400);
        roundPoints(poly_points, &poly_graphic_points);
    }

    if (atoi(argv[1]) < 7) {
        bmp_draw_points(&bmp, poly_graphic_points, white);
    } else {
        bmp_fill_points(&bmp, poly_graphic_points, white);
    }

    size_t bmp_size = bmp_calculate_size(&bmp);
    uint8_t *serialized_bmp = malloc(bmp_size);
    bmp_serialize(&bmp, serialized_bmp);

    image_server_set_data(bmp_size, serialized_bmp);
    image_server_start("8000");
    sleep(1);

    // write to a file so that we can check if the image is a valid image
    FILE *f = fopen("my_image.bmp", "wb");
    fwrite(serialized_bmp, bmp_size, 1, f);
    fclose(f);

    free(poly_points.buffer);
    free(poly_graphic_points.buffer);
    free(bmp.data);
    free(serialized_bmp);
    return 0;
}
