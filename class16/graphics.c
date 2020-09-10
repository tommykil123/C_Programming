#include "graphics.h"
#include <stdio.h>
#include <stdlib.h>

void updateGraphicsFinal(bitmap_t bmp) {
    image_server_start("8000");
    size_t bmp_size = bmp_calculate_size(&bmp);
    uint8_t *serialized_bmp = malloc(bmp_size);
    bmp_serialize(&bmp, serialized_bmp);
    image_server_set_data(bmp_size, serialized_bmp);
    sleep(1);
    // FILE *f = fopen("my_image.bmp", "wb");
    // fwrite(serialized_bmp, bmp_size, 1, f);
    // fclose(f);
    free(serialized_bmp);
    free(bmp.data);
}

void updateGraphics(bitmap_t bmp) {
    image_server_start("8000");
    size_t bmp_size = bmp_calculate_size(&bmp);
    uint8_t *serialized_bmp = malloc(bmp_size);
    bmp_serialize(&bmp, serialized_bmp);
    image_server_set_data(bmp_size, serialized_bmp);
    int seconds = 0;
    long nanoseconds = 40 * 1000 * 1000;
    struct timespec interval = {seconds, nanoseconds};
    nanosleep(&interval, NULL);
    free(serialized_bmp);
    free(bmp.data);
}

void drawRobot(bitmap_t *bmp, vector_t poly_pts, color_bgr_t color) {
    vector_t poly_pxs;
    init(&poly_pxs);
    roundPoints(poly_pts, &poly_pxs);
    bmp_fill_points(bmp, poly_pxs, color);
    // for (int i = 0; i < 3; i++) {
    //     printf("x: %f, y: %f\n", poly_pxs.buffer[i][0], poly_pxs.buffer[i][1]);
    // }
    free(poly_pxs.buffer_x);
    free(poly_pxs.buffer_y);
}

void plotLine(vector_t *data_points, int x0, int y0, int x1, int y1) {
    append(data_points, x0, y0);
    double dx = abs((double)x1 - (double)x0);
    int sx = x0 < x1 ? 1 : -1;
    double dy = -abs((double)y1 - (double)y0);
    int sy = y0 < y1 ? 1 : -1;
    double err = (double)dx + (double)dy;
    while (1) {
        if (x0 == x1 && y0 == y1) {
            break;
        }
        double e2 = 2 * err;
        if (e2 >= dy) {
            err += dy;
            x0 += sx;
        }
        if (e2 <= dx) {
            err += dx;
            y0 += sy;
        }
        append(data_points, x0, y0);
    }
}

void drawRectangle(vector_t *points, int w, int h) {
    append(points, (double)w, (double)0);
    append(points, (double)w, (double)h);
    append(points, (double)0, (double)h);
    append(points, (double)0, (double)0);
}

void drawTriangle(vector_t *points, double length) {
    double width = 3.0 * length / 4.0;
    double arrow_pt_x = length / 2.0;
    double arrow_pt_y = 0.0;
    double top_pt_x = -length / 2.0;
    double top_pt_y = width / 2.0;
    double bot_pt_x = -length / 2.0;
    double bot_pt_y = -width / 2.0;
    append(points, arrow_pt_x, arrow_pt_y);
    append(points, top_pt_x, top_pt_y);
    append(points, bot_pt_x, bot_pt_y);
    // append(points, arrow_pt_x, arrow_pt_y);
    // append(points, bot_pt_x, bot_pt_y);
    // append(points, top_pt_x, top_pt_y);
}

void translate(vector_t *points, double x_trans, double y_trans) {
    for (int i = 0; i < points->size; i++) {
        double x = points->buffer_x[i];
        double y = points->buffer_y[i];
        points->buffer_x[i] = x + x_trans;
        points->buffer_y[i] = y + y_trans;
    }
}

void rotate(vector_t *points, double angle) {
    // double theta = angle * PI / 180;
    double theta = angle;
    for (int i = 0; i < points->size; i++) {
        double x = points->buffer_x[i];
        double y = points->buffer_y[i];
        points->buffer_x[i] = x * cosf(theta) + y * sinf(theta);
        points->buffer_y[i] = x * -sinf(theta) + y * cosf(theta);
    }
}

void roundPoints(vector_t points, vector_t *rounded_points) {
    double min_x = 999.0;
    double min_y = 999.0;
    for (int i = 0; i < points.size; i++) {
        double x = points.buffer_x[i];
        double y = points.buffer_y[i];
        if (x <= min_x) {
            min_x = x;
        }
        if (y <= min_y) {
            min_y = y;
        }
    }
    for (int i = 0; i < points.size; i++) {
        double x = points.buffer_x[i];
        double y = points.buffer_y[i];
        double rounded_x, rounded_y;
        if (x == min_x) {
            rounded_x = ceilf(x);
        } else {
            rounded_x = floor(x - EPSILON);
        }
        if (y == min_y) {
            rounded_y = ceilf(y);
        } else {
            rounded_y = floor(y - EPSILON);
        }
        append(rounded_points, rounded_x, rounded_y);
    }
}

void bmp_draw_points(bitmap_t *bmp, vector_t points, color_bgr_t color) {
    vector_t tmp_points;
    init(&tmp_points);

    double x, y, x_next, y_next;
    for (int i = 0; i < points.size; i++) {
        x = points.buffer_x[i];
        y = points.buffer_y[1];
        if (i == (points.size - 1)) {
            x_next = points.buffer_x[0];
            y_next = points.buffer_y[1];
        } else {
            x_next = points.buffer_x[i + 1];
            y_next = points.buffer_y[i + 1];
        }
        plotLine(&tmp_points, x, y, x_next, y_next);
    }
    for (int i = 0; i < tmp_points.size; i++) {
        int x_int = (int)tmp_points.buffer_x[i];
        int y_int = (int)tmp_points.buffer_y[i];
        if (x_int >= 0 && y_int >= 0) {
            bmp->data[x_int + y_int * bmp->width] = color;
        }
    }
    free(tmp_points.buffer_x);
    free(tmp_points.buffer_y);
}

void bmp_fill_points(bitmap_t *bmp, vector_t points, color_bgr_t color) {
    vector_t tmp_points;
    init(&tmp_points);

    for (int i = 0; i < points.size; i++) {
        double x = points.buffer_x[i];
        double y = points.buffer_y[i];
        double x_next, y_next;
        if (i == (points.size - 1)) {
            x_next = points.buffer_x[0];
            y_next = points.buffer_y[0];
        } else {
            x_next = points.buffer_x[i + 1];
            y_next = points.buffer_y[i + 1];
        }
        plotLine(&tmp_points, x, y, x_next, y_next);
    }
    /*for (int i = 0; i < tmp_points.size; i++) {
        printf("x: %f, y: %f\n",tmp_points.buffer[i][0],tmp_points.buffer[i][1]);
    }*/
    int start_x[480] = {[0 ... 479] = -1};
    int end_x[480] = {[0 ... 479] = -1};
    for (int i = 0; i < tmp_points.size; i++) {
        int x = (int)tmp_points.buffer_x[i];
        int y = (int)tmp_points.buffer_y[i];
        if (y >= 0) {
            if (start_x[y] == -1) {
                start_x[y] = x;
                end_x[y] = x;
            } else {
                if (x < start_x[y]) {
                    start_x[y] = x;
                }
                if (x > end_x[y]) {
                    end_x[y] = x;
                }
            }
        }
    }
    for (int i = 0; i < 480; i++) {
        if ((start_x[i] != -1) || ((end_x[i] - start_x[i]) > 2)) {
            for (int j = start_x[i]; j < end_x[i]; j++) {
                append(&tmp_points, (double)j, (double)i);
            }
        }
    }
    for (int i = 0; i < tmp_points.size; i++) {
        int x_int = (int)tmp_points.buffer_x[i];
        int y_int = (int)tmp_points.buffer_y[i];
        if (x_int >= 0 && y_int >= 0) {
            bmp->data[x_int + y_int * bmp->width] = color;
        }
    }
    free(tmp_points.buffer_x);
    free(tmp_points.buffer_y);
}
