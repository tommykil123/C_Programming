#include "update.h"

double distance(double x0, double y0, double x1, double y1) {
    return sqrt(pow((x1 - x0), 2.0) + pow((y1 - y0), 2.0));
}

uvector_t calcUnitVector(double x0, double y0, double x1, double y1) {
    double norm = distance(x0, y0, x1, y1);
    uvector_t unit_vector = {0};
    unit_vector.x = (x1 - x0) / norm;
    unit_vector.y = (y1 - y0) / norm;
    return unit_vector;
}

vector_t vectRobot(agent_t robot) {
    vector_t poly_pts;
    init(&poly_pts);
    drawTriangle(&poly_pts, ROBOT_LENGTH);
    rotate(&poly_pts, robot.theta);
    translate(&poly_pts, robot.x, robot.y);
    return poly_pts;
}

void refreshSimulation(state_t *state) {
    srand(0);
    int initial_runner_idx = state->settings.initial_runner_idx;
    int idx_x = initial_runner_idx % MAP_W;
    int idx_y = initial_runner_idx / MAP_W;
    double runner_x = ((double)idx_x + 0.5) * BLOCK_SIZE;
    double runner_y = ((double)idx_y + 0.5) * BLOCK_SIZE;
    agent_t chaser = initChaser(320.0, 240.0, 0);
    agent_t runner = initRunner(runner_x, runner_y, 0.0);
    state->chaser = chaser;
    state->runner = runner;
    state->time_step = 0;
    state->count_delay_every = 1;
}

void refreshSelected(state_t *state) {
    state->selected.selected_init_idx = false;
    state->selected.selected_delay = false;
    state->selected.selected_2goal_mag = false;
    state->selected.selected_2goal_pow = false;
    state->selected.selected_avoid_mag = false;
    state->selected.selected_avoid_pow = false;
    state->selected.selected_max_vel = false;
}

void updateSelected(state_t *state) {
    if (state->selected.selected_idx == 0) {
        state->selected.selected_init_idx = true;
    } else if (state->selected.selected_idx == 1) {
        state->selected.selected_delay = true;
    } else if (state->selected.selected_idx == 2) {
        state->selected.selected_2goal_mag = true;
    } else if (state->selected.selected_idx == 3) {
        state->selected.selected_2goal_pow = true;
    } else if (state->selected.selected_idx == 4) {
        state->selected.selected_avoid_mag = true;
    } else if (state->selected.selected_idx == 5) {
        state->selected.selected_avoid_pow = true;
    } else if (state->selected.selected_idx == 6) {
        state->selected.selected_max_vel = true;
    } else {
        fprintf(stderr, "selected outside of idx range!\n");
        exit(1);
    }
}

void updateTune(state_t *state, char *action) {
    double sign = 0.0;
    if (!strcmp(action, "up")) {
        sign = 1.0;
    } else {
        sign = -1.0;
    }
    if (state->selected.selected_init_idx) {
        int initial_runner_idx = state->settings.initial_runner_idx;
        while (1) {
            initial_runner_idx += (int)sign;
            int idx_x = initial_runner_idx % MAP_W;
            int idx_y = initial_runner_idx / MAP_W;
            if (!(MAP[idx_y * MAP_W + idx_x] == 'X')) {
                state->settings.initial_runner_idx = initial_runner_idx;
                break;
            }
        }
    } else if (state->selected.selected_delay) {
        state->settings.delay_every += (int)sign;
        if (state->settings.delay_every < 1) {
            state->settings.delay_every = 1;
        }
    } else if (state->selected.selected_2goal_mag) {
        state->settings.to_goal_magnitude *= pow(2.0, sign);
    } else if (state->selected.selected_2goal_pow) {
        state->settings.to_goal_power += sign;
        if (state->settings.to_goal_power > 3.0) {
            state->settings.to_goal_power = 3.0;
        } else if (state->settings.to_goal_power < -3.0) {
            state->settings.to_goal_power = -3.0;
        }
    } else if (state->selected.selected_avoid_mag) {
        state->settings.avoid_obs_magnitude *= pow(2.0, sign);
    } else if (state->selected.selected_avoid_pow) {
        state->settings.avoid_obs_power += sign;
        if (state->settings.avoid_obs_power > 3.0) {
            state->settings.avoid_obs_power = 3.0;
        } else if (state->settings.avoid_obs_power < -3.0) {
            state->settings.avoid_obs_power = -3.0;
        }
    } else if (state->selected.selected_max_vel) {
        state->settings.max_vel += sign;
        if (state->settings.max_vel < 1.0) {
            state->settings.max_vel = 1.0;
        } else if (state->settings.max_vel > 12.0) {
            state->settings.max_vel = 12.0;
        }
    }
}

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
