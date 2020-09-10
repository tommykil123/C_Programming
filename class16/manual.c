#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "graphics.h"
#include "collision.h"
#include "init_chase.h"
#include "update.h"
#include <termios.h>
#include <pthread.h>

#define _GNU_SOURCE
#define PI 3.14159265
color_bgr_t white = {255, 255, 255};
color_bgr_t green = {0, 255, 0};
color_bgr_t red = {0, 0, 255};
color_bgr_t yellow = {0, 255, 255};
color_bgr_t black = {0, 0, 0};

int command;

struct termios original_termios;
void reset_terminal(void) {
    tcsetattr(STDIN_FILENO, TCSANOW, &original_termios);
}

void *io_thread(void *user) {
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
        int command = getc(stdin);
        if (command == 27) {
            int command_pt2 = getc(stdin);
            int command_pt3 = getc(stdin);
            if (command_pt2 == 91) {
                if (command_pt3 == 65) {
                    state->user_action = 1;
                } else if (command_pt3 == 66) {
                } else if (command_pt3 == 67) {
                    state->user_action = 3;
                } else if (command_pt3 == 68) {
                    state->user_action = 2;
                }
            }
        }
        if (command == 'q') {
            free(state->init_bmp.data);
            exit(0);
        }
        // printf("%c: %d\n", command, command);
    }
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

    // Initialize
    bitmap_t bmp = initBMP();
    agent_t chaser = initChaser(320.0, 240.0, 0);
    state_t state = initState(0, chaser, bmp);
    bitmap_t bmp_update;

    // Create IO Thread;
    pthread_t thread;
    pthread_create(&thread, NULL, io_thread, &state);

    // Main Thread
    while (true) {
        // ==============================================
        vector_t chaser_robot = vectRobot(state.chaser);
        if (enable) {
            bmp_update = initBMP();
            memcpy(bmp_update.data, state.init_bmp.data,
                   state.init_bmp.width * state.init_bmp.height * sizeof(color_bgr_t));
            drawRobot(&bmp_update, chaser_robot, red);
        }
        free(chaser_robot.buffer_x);
        free(chaser_robot.buffer_y);
        updateChaserState(&state, state.user_action);
        decayRobot(&state);
        updateState4Collision(&state);
        if (enable) {
            updateGraphics(bmp_update);
        }
        printf("%.2f %.2f\n", state.chaser.vel, state.chaser.ang_vel);
        // ==============================================
    }
    return 0;
}
