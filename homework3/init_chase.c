#include "init_chase.h"

state_t initState(int t, bool caught, agent_t r, agent_t c) {
    state_t state;
    state.time_step = t;
    state.runner_caught = caught;
    state.runner = r;
    state.chaser = c;
    return state;
}

bitmap_t initBMP(void) {
    color_bgr_t white = {255, 255, 255};
    color_bgr_t black = {0, 0, 0};
    bitmap_t bmp = {0};
    bmp.width = WIDTH;
    bmp.height = HEIGHT;
    bmp.data = calloc(bmp.width * bmp.height, sizeof(color_bgr_t));
    for (int i = 0; i < WIDTH * HEIGHT; i++) {
        bmp.data[i] = white;
    }
    for (int i = 0; i < HEIGHT / BLOCK_SIZE; i++) {
        for (int j = 0; j < WIDTH / BLOCK_SIZE; j++) {
            if (MAP[i * WIDTH / BLOCK_SIZE + j] == 'X') {
                vector_t poly_points;
                init(&poly_points);
                vector_t poly_graphic_points;
                init(&poly_graphic_points);
                drawRectangle(&poly_points, BLOCK_SIZE, BLOCK_SIZE);
                translate(&poly_points, j * BLOCK_SIZE, i * BLOCK_SIZE);
                roundPoints(poly_points, &poly_graphic_points);
                bmp_fill_points(&bmp, poly_graphic_points, black);
                free(poly_points.buffer_x);
                free(poly_points.buffer_y);
                free(poly_graphic_points.buffer_x);
                free(poly_graphic_points.buffer_y);
            }
        }
    }
    return bmp;
}

agent_t initRunner(double x, double y, double theta) {
    agent_t runner;
    runner.x = x;
    runner.y = y;
    runner.theta = theta;
    runner.vel = 0.0;
    runner.ang_vel = 0.0;
    return runner;
}

agent_t initChaser(double x, double y, double theta) {
    agent_t chaser;
    chaser.x = x;
    chaser.y = y;
    chaser.theta = theta;
    chaser.vel = 0.0;
    chaser.ang_vel = 0.0;
    return chaser;
}
