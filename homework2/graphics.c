#include "graphics.h"
#include <stdio.h>
#include <stdlib.h>

void plotLine(vector_t *data_points, int x0, int y0, int x1, int y1) {
    append(data_points, x0, y0);
    double dx = abs(x1 - x0);
    int sx = x0 < x1 ? 1 : -1;
    double dy = -abs(y1 - y0);
    int sy = y0 < y1 ? 1 : -1;
    double err = dx + dy;
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
    double min_x = -w / 2.0;
    double min_y = -h / 2.0;
    double max_x = w / 2.0;
    double max_y = h / 2.0;
    append(points, min_x, max_y);
    append(points, max_x, max_y);
    append(points, max_x, min_y);
    append(points, min_x, min_y);
}

void drawTriangle(vector_t *points, double length) {
    double width = 3 * length / 4;
    double arrow_pt_x = length / 2.0;
    double arrow_pt_y = 0.0;
    double top_pt_x = -length / 2.0;
    double top_pt_y = width / 2.0;
    double bot_pt_x = -length / 2.0;
    double bot_pt_y = -width / 2.0;
    append(points, bot_pt_x, bot_pt_y);
    append(points, arrow_pt_x, arrow_pt_y);
    append(points, top_pt_x, top_pt_y);
}

void translate(vector_t *points, double x_trans, double y_trans) {
    for (int i = 0; i < points->size; i++) {
        double x = points->buffer[i][0];
        double y = points->buffer[i][1];
        points->buffer[i][0] = x + x_trans;
        points->buffer[i][1] = y + y_trans;
    }
}

void rotate(vector_t *points, double angle_deg) {
    double theta = angle_deg * PI / 180;
    for (int i = 0; i < points->size; i++) {
        double x = points->buffer[i][0];
        double y = points->buffer[i][1];
        points->buffer[i][0] = x * cosf(theta) + y * sinf(theta);
        points->buffer[i][1] = x * -sinf(theta) + y * cosf(theta);
    }
}

void roundPoints(vector_t points, vector_t *rounded_points) {
    double min_x = 999.0;
    double min_y = 999.0;
    for (int i = 0; i < points.size; i++) {
        double x = points.buffer[i][0];
        double y = points.buffer[i][1];
        if (x <= min_x) {
            min_x = x;
        }
        if (y <= min_y) {
            min_y = y;
        }
    }
    for (int i = 0; i < points.size; i++) {
        double x = points.buffer[i][0];
        double y = points.buffer[i][1];
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
        x = points.buffer[i][0];
        y = points.buffer[i][1];
        if (i == (points.size - 1)) {
            x_next = points.buffer[0][0];
            y_next = points.buffer[0][1];
        } else {
            x_next = points.buffer[i + 1][0];
            y_next = points.buffer[i + 1][1];
        }
        plotLine(&tmp_points, x, y, x_next, y_next);
    }
    for (int i = 0; i < tmp_points.size; i++) {
        int x_int = (int)tmp_points.buffer[i][0];
        int y_int = (int)tmp_points.buffer[i][1];
        if (x_int >= 0 && y_int >= 0) {
            bmp->data[x_int + y_int * bmp->width] = color;
        }
    }
    free(tmp_points.buffer);
}

void bmp_fill_points(bitmap_t *bmp, vector_t points, color_bgr_t color) {
    vector_t tmp_points;
    init(&tmp_points);

    for (int i = 0; i < points.size; i++) {
        double x = points.buffer[i][0];
        double y = points.buffer[i][1];
        double x_next, y_next;
        if (i == (points.size - 1)) {
            x_next = points.buffer[0][0];
            y_next = points.buffer[0][1];
        } else {
            x_next = points.buffer[i + 1][0];
            y_next = points.buffer[i + 1][1];
        }
        plotLine(&tmp_points, x, y, x_next, y_next);
    }
    /*for (int i = 0; i < tmp_points.size; i++) {
        printf("x: %f, y: %f\n",tmp_points.buffer[i][0],tmp_points.buffer[i][1]);
    }*/
    int start_x[480] = {[0 ... 479] = -1};
    int end_x[480] = {[0 ... 479] = -1};
    for (int i = 0; i < tmp_points.size; i++) {
        int x = (int)tmp_points.buffer[i][0];
        int y = (int)tmp_points.buffer[i][1];
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
        int x_int = (int)tmp_points.buffer[i][0];
        int y_int = (int)tmp_points.buffer[i][1];
        if (x_int >= 0 && y_int >= 0) {
            bmp->data[x_int + y_int * bmp->width] = color;
        }
    }
    free(tmp_points.buffer);
}
