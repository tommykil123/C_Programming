#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <pthread.h>
#include "lcmtypes/action_t.h"
#include "lcmtypes/world_t.h"
#include "lcmtypes/agent_t.h"
#include "lcmtypes/settings_t.h"
#include "init.h"
#include "update.h"

double seconds_now(void) {
    struct timespec now;
    if (clock_gettime(CLOCK_MONOTONIC, &now)) {
        fprintf(stderr, "Retrieving system time failed.\n");
        exit(1);
    }
    return now.tv_sec + now.tv_nsec / 1000000000.0;
}

void receive_action(const lcm_recv_buf_t *rbuf, const char *channel,
                    const action_t *msg, void *user_data) {
    state_t *state = user_data;
    double prev_vel = state->chaser.vel;
    double curr_vel = msg->vel;
    state->chaser.vel = fmin(prev_vel + 2.0, curr_vel);

    double curr_ang_vel = msg->ang_vel;
    if (curr_ang_vel > PI / 16) {
        state->chaser.ang_vel = PI / 16;
    } else if (curr_ang_vel < -PI / 16) {
        state->chaser.ang_vel = -PI / 16;
    } else {
        state->chaser.ang_vel = msg->ang_vel;
    }
}

void receive_setttings(const lcm_recv_buf_t *rbuf, const char *channel,
                       const settings_t *msg, void *user_data) {
    state_t *state = user_data;
    state->settings.initial_runner_idx = msg->initial_runner_idx;
    state->settings.delay_every = msg->delay_every;
    state->settings.to_goal_magnitude = msg->to_goal_magnitude;
    state->settings.to_goal_power = msg->to_goal_power;
    state->settings.avoid_obs_magnitude = msg->avoid_obs_magnitude;
    state->settings.avoid_obs_power = msg->avoid_obs_power;
    state->settings.max_vel = msg->max_vel;
}

void publish_world(state_t *state) {
    world_t world;
    world.runner = state->runner;
    world.chaser = state->chaser;
    world_t_publish(state->lcm, "WORLD_tkil", &world);
}

int main(int argc, char **argv) {
    lcm_t *lcm = lcm_create(NULL);
    settings_t settings = initSettings();
    ui_selected_t selected = initSelected();
    // Input
    // int initial_runner_idx = tune_values.initial_runner_idx;
    int initial_runner_idx = 17;
    int idx_x = initial_runner_idx % MAP_W;
    int idx_y = initial_runner_idx / MAP_W;
    double runner_x = ((double)idx_x + 0.5) * BLOCK_SIZE;
    double runner_y = ((double)idx_y + 0.5) * BLOCK_SIZE;

    agent_t chaser = initChaser(320.0, 240.0, 0);
    agent_t runner = initRunner(runner_x, runner_y, 0.0);
    state_t state = initState(0, 0, runner, chaser, settings, selected, lcm);

    // Subscribe to ACTION
    action_t_subscribe(lcm, "ACTION_tkil", receive_action, &state);
    // Subscribe to SETTING
    settings_t_subscribe(lcm, "SETTINGS_tkil", receive_setttings, &state);
    while (true) {
        double start = seconds_now();
        lcm_handle_timeout(lcm, 1);
        updateChaserState(&state);
        updateRunnerState(&state);
        decayRobot(&state);
        updateState4Collision(&state);
        publish_world(&state);
        double time_elapsed = (seconds_now() - start);
        int seconds = 0;
        long nanoseconds = (NANO - (long)(time_elapsed * 1000)) * 1000 * 1000;
        struct timespec interval = {seconds, nanoseconds};
        nanosleep(&interval, NULL);
    }
    // action_t_unsubscribe(lcm, action_sub);
    // settings_t_unsubscribe(lcm, settings_sub);
    lcm_destroy(lcm);
    return 0;
}
