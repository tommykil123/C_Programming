#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
// #include <stdbool.h>
// #include "bmp.h"
#include "graphics.h"
#include "collision.h"
#include "init_chase.h"
#include "trajectory.h"
#include "update.h"

#define _GNU_SOURCE
#define PI 3.14159265
color_bgr_t white = {255, 255, 255};
color_bgr_t green = {0, 255, 0};
color_bgr_t red = {0, 0, 255};
color_bgr_t yellow = {0, 255, 255};
color_bgr_t black = {0, 0, 0};

int main(int argc, char **argv) {
    if (argc != 4) {
        fprintf(stderr, "usage: %s <time steps> <fast=0|1|2> <initial runner index>\n", argv[0]);
        exit(1);
    }
    // Inputs
    int time_steps = atoi(argv[1]);
    int fast_mode = atoi(argv[2]);
    int initial_runner_idx = atoi(argv[3]);
    int idx_x = initial_runner_idx % MAP_W;
    int idx_y = initial_runner_idx / MAP_W;
    double runner_x = ((double)idx_x + 0.5) * BLOCK_SIZE;
    double runner_y = ((double)idx_y + 0.5) * BLOCK_SIZE;
    // Initialize
    bitmap_t bmp = initBMP();
    agent_t chaser = initChaser(320.0, 240.0, 0);
    agent_t runner = initRunner(runner_x, runner_y, 0.0);
    state_t state = initState(0, 0, runner, chaser);
    bitmap_t bmp_update;
    for (int i = 0; i < time_steps + 1; i++) {
        state.time_step = i;
        vector_t chaser_robot = vectRobot(state.chaser);
        vector_t runner_robot = vectRobot(state.runner);
        if (!fast_mode || i == time_steps) {
            if (fast_mode != 2) {
                bmp_update = initBMP();
                memcpy(bmp_update.data, bmp.data, bmp.width * bmp.height * sizeof(color_bgr_t));
                drawRobot(&bmp_update, chaser_robot, red);
                drawRobot(&bmp_update, runner_robot, green);
            }
        }
        int collision_robot = collision(runner_robot, chaser_robot);
        free(chaser_robot.buffer_x);
        free(chaser_robot.buffer_y);
        free(runner_robot.buffer_x);
        free(runner_robot.buffer_y);
        if (collision_robot) {
            if (fast_mode != 2) {
                updateGraphics(bmp_update);
            }
            break;
        }
        // printf("%d: ", state.time_step);
        // printf("x: %.2f ", state.chaser.x);
        // printf("y: %.2f ", state.chaser.y);
        // printf("theta: %.2f ", state.chaser.theta);
        // printf("ang: %.2f ", state.chaser.vel);
        // printf("ang_vel: %.2f\n", state.chaser.ang_vel);
        int runner_action = updateRunnerState(&state);
        int chaser_action = updateChaserState(&state);
        printf("%d %d\n", runner_action, chaser_action);
        decayRobot(&state);
        updateState4Collision(&state);
        if (!fast_mode) {
            updateGraphics(bmp_update);
        } else if (fast_mode == 1) {
            if (i == time_steps) {
                updateGraphics(bmp_update);
            }
        }
    }
    free(bmp.data);
    return 0;
}
