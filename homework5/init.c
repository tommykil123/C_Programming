#include "init.h"

ui_selected_t initSelected(void) {
    ui_selected_t selected;
    selected.selected_init_idx = true; // 0
    selected.selected_delay = false; // 1
    selected.selected_2goal_mag = false; // 2
    selected.selected_2goal_pow = false; // 3
    selected.selected_avoid_mag = false; // 4
    selected.selected_avoid_pow = false; // 5
    selected.selected_max_vel = false; // 6
    selected.selected_idx = 0;
    return selected;
}

tune_values_t initTuneValues(void) {
    tune_values_t tunes;
    tunes.initial_runner_idx = 17;
    // tunes.initial_runner_idx = 86;
    tunes.delay_every = 1;
    tunes.to_goal_magnitude = 50.0;
    // tunes.to_goal_magnitude = 100.0;
    tunes.to_goal_power = 0.0;
    tunes.avoid_obs_magnitude = 1.0;
    tunes.avoid_obs_power = -2.0;
    tunes.max_velocity = 12.0;
    tunes.b_run = true;
    tunes.num_parameter = 7;
    return tunes;
}

state_t initState(int t, bool caught, agent_t r, agent_t c,
                  tune_values_t tune_values, ui_selected_t selected) {
    state_t state;
    state.time_step = t;
    state.runner_caught = caught;
    state.runner = r;
    state.chaser = c;
    state.tune_values = tune_values;
    state.selected = selected;
    state.count_delay_every = 1;
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
    for (int i = 0; i < MAP_H; i++) {
        for (int j = 0; j < MAP_W; j++) {
            if (MAP[i * MAP_W + j] == 'X') {
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
