#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "bmp.h"
#include "image_server.h"
#include "graphics.h"
#include <math.h>
#define _GNU_SOURCE
#include <time.h>
#include "collision.h"

#define LAMP_POWER 100000.0
#define MAX_PX_DT 12.0
#define WHL_BASE 80.0
#define PI 3.14159265

const double lamps_x[3] = {124.1, 349.1, 449.1};
const double lamps_y[3] = {224.1, 99.1, 349.1};

double MAX(double x, double y) {
    return (x > y) ? x : y;
}

double MIN(double x, double y) {
    return (x < y) ? x : y;
}

typedef struct uvector {
    double x;
    double y;
} uvector_t;

double distance(double x0, double y0, double x1, double y1) {
    return sqrt(pow((x1 - x0), 2) + pow((y1 - y0), 2));
}

uvector_t calcUnitVector(double x0, double y0, double x1, double y1) {
    double norm = distance(x0, y0, x1, y1);
    uvector_t unit_vector = {0};
    unit_vector.x = (x1 - x0) / norm;
    unit_vector.y = (y1 - y0) / norm;
    return unit_vector;
}

double dotProduct(uvector_t u, uvector_t v) {
    double x_comp = u.x * v.x;
    double y_comp = u.y * v.y;
    return (x_comp + y_comp);
}

bitmap_t initBMP(void) {
    bitmap_t bmp = {0};
    bmp.width = 640;
    bmp.height = 480;
    bmp.data = calloc(bmp.width * bmp.height, sizeof(color_bgr_t));
    return bmp;
}

vector_t createLampVector(bitmap_t *bmp, double x, double y, color_bgr_t color) {
    vector_t lamp_pts, lamp_pxs;
    init(&lamp_pts);
    init(&lamp_pxs);
    drawRectangle(&lamp_pts, 12, 12);
    rotate(&lamp_pts, 45.0);
    translate(&lamp_pts, x, y);
    roundPoints(lamp_pts, &lamp_pxs);
    bmp_fill_points(bmp, lamp_pxs, color);
    free(lamp_pxs.buffer);
    return lamp_pts;
}

vector_t createWallVector(bitmap_t *bmp, color_bgr_t color) {
    vector_t rect_pts, rect_pxs;
    init(&rect_pts);
    init(&rect_pxs);
    drawRectangle(&rect_pts, 600, 440);
    translate(&rect_pts, 640.0 / 2.0, 480.0 / 2.0);
    roundPoints(rect_pts, &rect_pxs);
    bmp_draw_points(bmp, rect_pxs, color);
    free(rect_pxs.buffer);
    return rect_pts;
}

vector_t updateRobot(double x, double y, double theta) {
    vector_t poly_pts;
    init(&poly_pts);
    drawTriangle(&poly_pts, 28.0);
    rotate(&poly_pts, theta);
    translate(&poly_pts, x, y);
    return poly_pts;
}

void drawRobot(bitmap_t *bmp, vector_t poly_pts, color_bgr_t color) {
    vector_t poly_pxs;
    init(&poly_pxs);
    roundPoints(poly_pts, &poly_pxs);
    bmp_fill_points(bmp, poly_pxs, color);
    for (int i = 0; i < 3; i++) {
        printf("x: %f, y: %f\n", poly_pxs.buffer[i][0], poly_pxs.buffer[i][1]);
    }
    free(poly_pxs.buffer);
}

void updateGraphics(bitmap_t bmp) {
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

void updateGraphicsFinal(bitmap_t bmp) {
    size_t bmp_size = bmp_calculate_size(&bmp);
    uint8_t *serialized_bmp = malloc(bmp_size);
    bmp_serialize(&bmp, serialized_bmp);
    image_server_set_data(bmp_size, serialized_bmp);
    sleep(1);
    free(serialized_bmp);
    free(bmp.data);
}

int checkCollision(vector_t r, vector_t l0, vector_t l1, vector_t l2) {
    int lamp_collision0 = collision(r, l0);
    int lamp_collision1 = collision(r, l1);
    int lamp_collision2 = collision(r, l2);
    if (lamp_collision0) {
        return 0;
    }
    if (lamp_collision1) {
        return 1;
    }
    if (lamp_collision2) {
        return 2;
    }
    return -1;
}

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "usage: %s <time steps> <fast=0|1>\n", argv[0]);
        exit(1);
    }
    int time_steps = atoi(argv[1]);
    int fast_mode = atoi(argv[2]);
    color_bgr_t white = {255, 255, 255}, green = {0, 255, 0}, yellow = {0, 255, 255};
    image_server_start("8000");
    double x = 320.0, y = 240.0, theta_rad = 0;
    bitmap_t bmp = initBMP();
    vector_t lamp_00 = createLampVector(&bmp, lamps_x[0], lamps_y[0], yellow);
    vector_t lamp_01 = createLampVector(&bmp, lamps_x[1], lamps_y[1], yellow);
    vector_t lamp_02 = createLampVector(&bmp, lamps_x[2], lamps_y[2], yellow);
    vector_t wall = createWallVector(&bmp, white);

    bitmap_t bmp_update;
    for (int i = 0; i < time_steps + 1; i++) {
        /*if (i == 250 && time_steps == 250) {
            x -= 0.5;
        }*/
        double move_l = 0;
        double move_r = 0;
        vector_t robot = updateRobot(x, y, theta_rad * 180 / PI);
        if (!fast_mode || i == time_steps) {
            bmp_update = initBMP();
            memcpy(bmp_update.data, bmp.data, bmp.width * bmp.height * sizeof(color_bgr_t));
            drawRobot(&bmp_update, robot, green);
        }
        for (int j = 0; j < sizeof(lamps_x) / sizeof(double); j++) {
            double dist = distance(x, y, lamps_x[j], lamps_y[j]);
            double dist_sq = pow(dist, 2);
            double theta_plus_60 = -theta_rad + 60.0 * PI / 180.0;
            double theta_minus_60 = -theta_rad - 60.0 * PI / 180.0;
            uvector_t dir = calcUnitVector(x, y, lamps_x[j], lamps_y[j]);
            uvector_t eye_l = {cosf(theta_plus_60), sinf(theta_plus_60)};
            uvector_t eye_r = {cosf(theta_minus_60), sinf(theta_minus_60)};
            move_l += MAX(0.0, dotProduct(dir, eye_l)) * LAMP_POWER / dist_sq;
            move_r += MAX(0.0, dotProduct(dir, eye_r)) * LAMP_POWER / dist_sq;
        }
        move_l = MIN(MAX_PX_DT, move_l);
        move_r = MIN(MAX_PX_DT, move_r);
        theta_rad += (move_r - move_l) / WHL_BASE;
        double forward_dist = (move_r + move_l) / 2.0;
        x += forward_dist * cosf(theta_rad);
        y += forward_dist * -sinf(theta_rad);

        uvector_t dir;
        int first_collision = 1;
        while (1) {
            vector_t robot_next = updateRobot(x, y, theta_rad * 180 / PI);
            int col_idx = checkCollision(robot_next, lamp_00, lamp_01, lamp_02);
            if (col_idx != -1) {
                if (first_collision) {
                    dir = calcUnitVector(x, y, lamps_x[col_idx], lamps_y[col_idx]);
                    //first_collision = 0;
                }
                x -= 0.5 * dir.x;
                y -= 0.5 * dir.y;
                free(robot_next.buffer);
            } else {
                free(robot_next.buffer);
                break;
            }
        }

        if (!fast_mode) {
            updateGraphics(bmp_update);
        } else {
            if (i == time_steps) {
                updateGraphicsFinal(bmp_update);
            }
        }
        free(robot.buffer);
    }
    free(lamp_00.buffer);
    free(lamp_01.buffer);
    free(lamp_02.buffer);
    free(wall.buffer);
    free(bmp.data);
    return 0;
}
