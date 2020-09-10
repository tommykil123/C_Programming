#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "graphics.h"
#include "init.h"
#include "update.h"
#include <termios.h>
#include <pthread.h>
#include "lcmtypes/settings_t.h"
#include "lcmtypes/reset_t.h"
#include "lcmtypes/world_t.h"
#include "lcmtypes/agent_t.h"

#define _GNU_SOURCE
#define PI 3.14159265
#define HIGHLIGHT "\e[7m"
#define CLEAR_HIGHLIGHT "\e[0m"

color_bgr_t white = {255, 255, 255};
color_bgr_t green = {0, 255, 0};
color_bgr_t red = {0, 0, 255};
color_bgr_t yellow = {0, 255, 255};
color_bgr_t black = {0, 0, 0};

int command;

struct termios original_termios;

typedef struct user_data {
    int32_t initial_runner_idx;
    int32_t delay_every;
    double to_goal_magnitude;
    int32_t to_goal_power;
    double avoid_obs_magnitude;
    int32_t avoid_obs_power;
    double max_vel;
} user_data_t;

double seconds_now(void) {
    struct timespec now;
    if (clock_gettime(CLOCK_MONOTONIC, &now)) {
        fprintf(stderr, "Retrieving system time failed.\n");
        exit(1);
    }
    return now.tv_sec + now.tv_nsec / 1000000000.0;
}

void print_settings(const lcm_recv_buf_t *rbuf, const char *channel,
                    const settings_t *msg, void *userdata) {
    printf("\ninitial_idx: %d\n", msg->initial_runner_idx);
}

void publish_reset(state_t *state) {
    reset_t reset;
    reset.initial_runner_idx = state->settings.initial_runner_idx;
    reset_t_publish(state->lcm, "RESET_tkil", &reset);
}

void reset_terminal(void) {
    printf("\e[?25h");
    tcsetattr(STDIN_FILENO, TCSANOW, &original_termios);
}

void print_ui(state_t state) {
    printf("\r%s %s%d%s %s %s%d%s %s %s%.2f%s %s %s%.0d%s %s %s%.2f%s %s %s%.0d%s %s %s%.0f%s",
           "idx:", state.selected.selected_init_idx ? HIGHLIGHT : "",
           state.settings.initial_runner_idx, CLEAR_HIGHLIGHT,
           "del:", state.selected.selected_delay ? HIGHLIGHT : "",
           state.settings.delay_every, CLEAR_HIGHLIGHT,
           "g_mag:", state.selected.selected_2goal_mag ? HIGHLIGHT : "",
           state.settings.to_goal_magnitude, CLEAR_HIGHLIGHT,
           "g_pow:", state.selected.selected_2goal_pow ? HIGHLIGHT : "",
           state.settings.to_goal_power, CLEAR_HIGHLIGHT,
           "a_mag:", state.selected.selected_avoid_mag ? HIGHLIGHT : "",
           state.settings.avoid_obs_magnitude, CLEAR_HIGHLIGHT,
           "a_pow:", state.selected.selected_avoid_pow ? HIGHLIGHT : "",
           state.settings.avoid_obs_power, CLEAR_HIGHLIGHT,
           "max_v:", state.selected.selected_max_vel ? HIGHLIGHT : "",
           state.settings.max_vel, CLEAR_HIGHLIGHT);
}

void update_bmp_graphic(bitmap_t bmp, bitmap_t *bmp_update, vector_t chaser, vector_t runner) {
    *bmp_update = initBMP();
    memcpy(bmp_update->data, bmp.data, bmp.width * bmp.height * sizeof(color_bgr_t));
    drawRobot(bmp_update, chaser, red);
    drawRobot(bmp_update, runner, green);
    updateGraphics(*bmp_update);
}

void *io_thread(void *user) {
    printf("\e[?25l");
    state_t *state = user;

    // we need to return the terminal to its original state when our program quits
    // if we don't the terminal will not behave correctly
    tcgetattr(0, &original_termios);
    atexit(reset_terminal);

    // we start with the old settings, since we are only changing a few things
    struct termios new_termios;
    memcpy(&new_termios, &original_termios, sizeof(new_termios));

    // we turn off the bits for echo (show typed characters on the screen)
    // and canonical mode, which waits until you press newline to give the program input
    new_termios.c_lflag &= ~(ECHO | ICANON);
    // we are modifying the input settings, (standard in, stdin)
    // and we want the changes to take effect right now!
    tcsetattr(STDIN_FILENO, TCSANOW, &new_termios);

    while (true) {
        // put the rest of your code here!
        // read characters with getc(stdin)
        command = getc(stdin);
        if (command == 27) {
            int command_pt2 = getc(stdin);
            int command_pt3 = getc(stdin);
            if (command_pt2 == 91) {
                if (command_pt3 == 65) { // up
                    updateTune(state, "up");
                    refreshSimulation(state);
                    state->count_delay_every = 1;
                } else if (command_pt3 == 66) { // down
                    updateTune(state, "down");
                    refreshSimulation(state);
                    state->count_delay_every = 1;
                } else if (command_pt3 == 67) { // right
                    refreshSelected(state);
                    int curr_idx = state->selected.selected_idx;
                    state->selected.selected_idx = (curr_idx + 1) % 7;
                    updateSelected(state);
                } else if (command_pt3 == 68) { // left
                    refreshSelected(state);
                    int curr_idx = state->selected.selected_idx;
                    if (curr_idx == 0) {
                        state->selected.selected_idx = 7 - 1;
                    } else {
                        state->selected.selected_idx = (curr_idx - 1) % 7;
                    }
                    updateSelected(state);
                }
                // refreshSimulation(state);
            }
        }
        if (command == 'q') {
            exit(0);
        }
        if (command == 'r') {
            refreshSimulation(state);
            publish_reset(state);
        }
        // printf("%c: %d\n", command, command);
    }
}

void publish_settings(state_t *state) {
    settings_t settings;
    settings.initial_runner_idx = state->settings.initial_runner_idx;
    settings.delay_every = state->settings.delay_every;
    settings.to_goal_magnitude = state->settings.to_goal_magnitude;
    settings.to_goal_power = state->settings.to_goal_power;
    settings.avoid_obs_magnitude = state->settings.avoid_obs_magnitude;
    settings.avoid_obs_power = state->settings.avoid_obs_power;
    settings.max_vel = state->settings.max_vel;
    settings_t_publish(state->lcm, "SETTINGS_tkil", &settings);
}

void receive_world(const lcm_recv_buf_t *rbuf, const char *channel,
                   const world_t *msg, void *user_data) {
    state_t *state = user_data;
    state->chaser = msg->chaser;
    state->runner = msg->runner;
}

int main(int argc, char **argv) {
    if (argc > 2) {
        fprintf(stderr, "usage: %s <disable>", argv[0]);
        exit(1);
    }
    // Inputs
    bool enable = true;
    if (argc == 2) {
        if (!strcmp(argv[1], "disable")) {
            enable = false;
        }
    }
    settings_t settings_values = initSettings();
    ui_selected_t selected = initSelected();
    // Input
    int initial_runner_idx = settings_values.initial_runner_idx;
    int idx_x = initial_runner_idx % MAP_W;
    int idx_y = initial_runner_idx / MAP_W;
    double runner_x = ((double)idx_x + 0.5) * BLOCK_SIZE;
    double runner_y = ((double)idx_y + 0.5) * BLOCK_SIZE;
    // Initialize
    bitmap_t bmp = initBMP();
    agent_t chaser = initChaser(320.0, 240.0, 0);
    agent_t runner = initRunner(runner_x, runner_y, 0.0);
    lcm_t *lcm = lcm_create(NULL);
    state_t state = initState(0, 0, runner, chaser, settings_values, selected, lcm);
    bitmap_t bmp_update;
    // int count_delay_every = 1;
    // Create IO Thread;
    pthread_t thread;
    pthread_create(&thread, NULL, io_thread, &state);
    // Subscribe to WORLD
    world_t_subscribe(lcm, "WORLD_tkil", receive_world, &state);
    // Running Main Thread
    while (1) {
        double start = seconds_now();
        vector_t chaser_robot = vectRobot(state.chaser);
        vector_t runner_robot = vectRobot(state.runner);

        if (state.count_delay_every % state.settings.delay_every == 0) {
            if (enable) {
                update_bmp_graphic(bmp, &bmp_update, chaser_robot, runner_robot);
            }
            state.count_delay_every = 1;
            // print_ui(state);
            // fflush(stdout);
        }
        state.count_delay_every += 1;
        state.time_step += 1;
        // printf("%.2f\n", state.runner.x);
        free(chaser_robot.buffer_x);
        free(chaser_robot.buffer_y);
        free(runner_robot.buffer_x);
        free(runner_robot.buffer_y);

        publish_settings(&state);

        lcm_handle_timeout(lcm, 1);
        double time_elapsed = (seconds_now() - start);
        int seconds = 0;
        long nanoseconds = (NANO - (long)(time_elapsed * 1000)) * 1000 * 1000;
        struct timespec interval = {seconds, nanoseconds};
        nanosleep(&interval, NULL);
    }
    // world_t_unsubscribe(lcm, world_sub);
    lcm_destroy(lcm);
    free(bmp.data);
    return 0;
}
