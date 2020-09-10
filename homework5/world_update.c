#ifndef _WOLRD_UPDATEH_
#define _WOLRD_UPDATEH_


void updateRunnerState(state_t *state) {
    // Update the runner
    int rand_int = rand() % (19 + 1 - 0) + 0;
    if (rand_int == 1) {
        state->runner.vel += 2.0;
        if (state->runner.vel > MAX_VEL) {
            state->runner.vel = MAX_VEL;
        }
    } else if (rand_int == 2) {
        state->runner.ang_vel += PI / 16.0;
    } else {
        // rand_int = 0;
    }
    state->runner.theta += state->runner.ang_vel;
    state->runner.x += state->runner.vel * cos(state->runner.theta);
    state->runner.y -= state->runner.vel * sin(state->runner.theta);
}

void updateChaserState(state_t *state) {
    // printf("theta: %.2f ", state->chaser.theta);
    potential_field(state);
    state->chaser.theta += state->chaser.ang_vel;
    if (state->chaser.theta < -PI) {
        state->chaser.theta += 2.0 * PI;
    } else if (state->chaser.theta > PI) {
        state->chaser.theta -= 2.0 * PI;
    }
    state->chaser.x += state->chaser.vel * cos(state->chaser.theta);
    state->chaser.y -= state->chaser.vel * sin(state->chaser.theta);
    // printf("ang_vel: %.2f ", state->chaser.ang_vel);
    // printf("after theta: %.2f\n", state->chaser.theta);
}

void decayRobot(state_t *state) {
    state->runner.ang_vel *= 0.8;
    state->chaser.ang_vel *= 0.8;
}

bool checkWallCollision(state_t *state, char *choice) {
    bool b_wallcollision = false;
    agent_t robot;
    if (!strcmp(choice, "chaser")) {
        robot = state->chaser;
    } else if (!strcmp(choice, "runner")) {
        robot = state->runner;
    } else {
        fprintf(stderr, "no proper choice for checkWallCollision");
        exit(1);
    }
    int *wall_loc = malloc(sizeof(*wall_loc) * 2);
    wall_loc[0] = -1;
    wall_loc[1] = -1;
    // Robot location in terms of BLOCK SIZE
    int robot_x = floor(robot.x / BLOCK_SIZE);
    int robot_y = floor(robot.y / BLOCK_SIZE);
    int min_x = ((robot_x - 1) < 0) ? (0) : (robot_x - 1);
    int max_x = ((robot_x + 1) > (MAP_W)) ? (MAP_W) : (robot_x + 1);
    int min_y = ((robot_y - 1) < 0) ? (0) : (robot_y - 1);
    int max_y = ((robot_y + 1) > (MAP_H)) ? (MAP_H) : (robot_y + 1);
    for (int i = min_y; i < max_y + 1; i++) {
        for (int j = min_x; j < max_x + 1; j++) {
            if (MAP[i * MAP_W + j] == 'X') {
                vector_t poly_points;
                init(&poly_points);
                drawRectangle(&poly_points, BLOCK_SIZE, BLOCK_SIZE);
                translate(&poly_points, j * BLOCK_SIZE, i * BLOCK_SIZE);
                vector_t robot_points = vectRobot(robot);
                int collision_wall = collision(robot_points, poly_points);
                free(poly_points.buffer_x);
                free(poly_points.buffer_y);
                free(robot_points.buffer_x);
                free(robot_points.buffer_y);
                if (collision_wall) {
                    b_wallcollision = true;
                    wall_loc[0] = j * (int)BLOCK_SIZE + ((int)BLOCK_SIZE / 2);
                    wall_loc[1] = i * (int)BLOCK_SIZE + ((int)BLOCK_SIZE / 2);
                    if (!strcmp(choice, "chaser")) {
                        uvector_t dir = calcUnitVector(state->chaser.x, state->chaser.y,
                                                       (double)wall_loc[0], (double)wall_loc[1]);
                        state->chaser.x -= 0.5 * dir.x;
                        state->chaser.y -= 0.5 * dir.y;
                    } else if (!strcmp(choice, "runner")) {
                        uvector_t dir = calcUnitVector(state->runner.x, state->runner.y,
                                                       (double)wall_loc[0], (double)wall_loc[1]);
                        state->runner.x -= 0.5 * dir.x;
                        state->runner.y -= 0.5 * dir.y;
                    } else {
                        fprintf(stderr, "no proper choice for checkWallCollision");
                        exit(1);
                    }
                    // break;
                }
            }
        }
    }
    free(wall_loc);
    return b_wallcollision;
}

void updateState4Collision(state_t *state) {
    // Runner
    int collision_runner = 0;
    while (true) {
        bool b_wallcollision = checkWallCollision(state, "runner");
        if (!b_wallcollision) {
            break;
        }
        collision_runner = 1;
    }
    if (collision_runner) {
        state->runner.vel *= 0.25;
    }
    // Chaser
    int collision_chaser = 0;
    while (true) {
        bool b_wallcollision = checkWallCollision(state, "chaser");
        if (!b_wallcollision) {
            break;
        }
        collision_chaser = 1;
    }
    if (collision_chaser) {
        state->chaser.vel *= 0.25;
    }
}
#endif
