#ifndef _INIT_CHASEH_
#define _INIT_CHASEH_

#include <stdlib.h>
#include <stdbool.h>
#include "bmp.h"
#include "graphics.h"

#define FORTY 40
#define NANO FORTY
#define WIDTH 640
#define HEIGHT 480
#define BLOCK_SIZE ((double)FORTY)
#define MAP "XXXXXXXXXXXXXXXX" \
            "X              X" \
            "X  XXXX   XXX  X" \
            "X   XX      X  X" \
            "X       XXX    X" \
            "XXXXXX         X" \
            "X         XXXXXX" \
            "X    XXX       X" \
            "X  XX     XX   X" \
            "X   X    XX    X" \
            "X      XXX     X" \
            "XXXXXXXXXXXXXXXX"

#define MAP_W (int)(WIDTH / BLOCK_SIZE)
#define MAP_H (int)(HEIGHT / BLOCK_SIZE)

typedef struct ui_selected {
    bool selected_init_idx;
    bool selected_delay;
    bool selected_2goal_mag;
    bool selected_2goal_pow;
    bool selected_avoid_mag;
    bool selected_avoid_pow;
    bool selected_max_vel;
    int selected_idx;
} ui_selected_t;

typedef struct tune_values {
    int initial_runner_idx;
    int delay_every;
    double to_goal_magnitude;
    double to_goal_power;
    double avoid_obs_magnitude;
    double avoid_obs_power;
    double max_velocity;
    bool b_run;
    int num_parameter;
} tune_values_t;

typedef struct agent {
    double x;
    double y;
    double theta;
    double vel;
    double ang_vel;
} agent_t;

typedef struct state {
    int time_step;
    bool runner_caught;
    agent_t runner;
    agent_t chaser;
    tune_values_t tune_values;
    ui_selected_t selected;
    int count_delay_every;
} state_t;

ui_selected_t initSelected(void);
tune_values_t initTuneValues(void);
state_t initState(int t, bool caught, agent_t r, agent_t c,
                  tune_values_t tune_values, ui_selected_t selected);
bitmap_t initBMP(void);
agent_t initRunner(double x, double y, double theta);
agent_t initChaser(double x, double y, double theta);

#endif
