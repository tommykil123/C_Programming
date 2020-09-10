#ifndef _INIT_CHASEH_
#define _INIT_CHASEH_

#include <stdlib.h>
#include <stdbool.h>
#include "bmp.h"
#include "graphics.h"

#define WIDTH 640
#define HEIGHT 480
#define BLOCK_SIZE 40
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

#define MAP_W (WIDTH / BLOCK_SIZE)
#define MAP_H (HEIGHT / BLOCK_SIZE)

typedef struct agent {
    double x;
    double y;
    double theta;
    double vel;
    double ang_vel;
} agent_t;

typedef struct state {
    int time_step;
    agent_t chaser;
    bitmap_t init_bmp;
    int user_action;
} state_t;

state_t initState(int t, agent_t c, bitmap_t init_bmp);
bitmap_t initBMP(void);
agent_t initChaser(double x, double y, double theta);

#endif
