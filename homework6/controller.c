#include <stdio.h>
#include <stdlib.h>
#include "lcmtypes/action_t.h"
#include "lcmtypes/world_t.h"
#include "lcmtypes/agent_t.h"
#include "lcmtypes/settings_t.h"
#include "init.h"
#include "potential_controller.h"

double seconds_now(void) {
    struct timespec now;
    if (clock_gettime(CLOCK_MONOTONIC, &now)) {
        fprintf(stderr, "Retrieving system time failed.\n");
        exit(1);
    }
    return now.tv_sec + now.tv_nsec / 1000000000.0;
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

void receive_world(const lcm_recv_buf_t *rbuf, const char *channel,
                   const world_t *msg, void *user_data) {
    state_t *state = user_data;
    state->chaser = msg->chaser;
    state->runner = msg->runner;
}

void publish_action(state_t *state) {
    action_t action;
    action.vel = state->chaser.vel;
    action.ang_vel = state->chaser.ang_vel;
    action_t_publish(state->lcm, "ACTION_tkil", &action);
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

    // Subscribe to SETTING
    settings_t_subscribe(lcm, "SETTINGS_tkil", receive_setttings, &state);
    // Subscribe to WORLD
    world_t_subscribe(lcm, "WORLD_tkil", receive_world, &state);
    while (true) {
        double start = seconds_now();
        // lcm_handle_timeout(lcm, 1);
        lcm_handle(lcm);
        // Publish to ACTION
        potential_field(&state);
        publish_action(&state);

        double time_elapsed = (seconds_now() - start);
        int seconds = 0;
        long nanoseconds = (NANO - (long)(time_elapsed * 1000)) * 1000 * 1000;
        struct timespec interval = {seconds, nanoseconds};
        nanosleep(&interval, NULL);
    }
    // settings_t_unsubscribe(lcm, settings_sub);
    // world_t_unsubscribe(lcm, world_sub);
    lcm_destroy(lcm);
    return 0;
}
