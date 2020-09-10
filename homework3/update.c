#include "update.h"
#define MAX_VEL 12.0

double distance(double x0, double y0, double x1, double y1) {
    return sqrt(pow((x1 - x0), 2) + pow((y1 - y0), 2));
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
    drawTriangle(&poly_pts, 20.0 * 4.0 / 3.0);
    rotate(&poly_pts, robot.theta);
    translate(&poly_pts, robot.x, robot.y);
    return poly_pts;
}

int updateRunnerState(state_t *state) {
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
        rand_int = 0;
    }
    state->runner.theta += state->runner.ang_vel;
    state->runner.x += state->runner.vel * cosf(state->runner.theta);
    state->runner.y -= state->runner.vel * sinf(state->runner.theta);
    return rand_int;
}

int updateChaserState(state_t *state) {
    // Create a quad search tree
    qst_t *qst = qst_create();
    qst_initialize(qst, *state);
    qst_add(qst->node, 0);
    // Determine which action to go
    int best_action = 0;
    qst_search_action(*qst->node, &best_action);
    qst_destroy(qst);
    // Do the action
    // if (state->time_step == 0) {
    //     best_action = 2;
    // } else {
    //     best_action = 1;
    // }
    if (best_action == 0) {
    } else if (best_action == 1) {
        state->chaser.vel += 2.0;
        if (state->chaser.vel > MAX_VEL) {
            state->chaser.vel = MAX_VEL;
        }
    } else if (best_action == 2) {
        state->chaser.ang_vel += PI / 16.0;
    } else if (best_action == 3) {
        state->chaser.ang_vel -= PI / 16.0;
    } else {
        fprintf(stderr, "updateChaserState wrong state variable for best action (not in range)");
    }
    state->chaser.theta += state->chaser.ang_vel;
    state->chaser.x += state->chaser.vel * cosf(state->chaser.theta);
    state->chaser.y -= state->chaser.vel * sinf(state->chaser.theta);
    return best_action;
}

void decayRobot(state_t *state) {
    state->runner.ang_vel *= 0.8;
    state->chaser.ang_vel *= 0.8;
}

int *checkWallCollision(agent_t robot) {
    int *wall_loc = malloc(sizeof(*wall_loc) * 2);
    wall_loc[0] = -1;
    wall_loc[1] = -1;
    // Robot location in terms of BLOCK SIZE
    int robot_x = floor(robot.x / BLOCK_SIZE);
    int robot_y = floor(robot.y / BLOCK_SIZE);
    int min_x = ((robot_x - 1) < 0) ? (0) : (robot_x - 1);
    int max_x = ((robot_x + 1) > (WIDTH / BLOCK_SIZE)) ? (WIDTH / BLOCK_SIZE) : (robot_x + 1);
    int min_y = ((robot_y - 1) < 0) ? (0) : (robot_y - 1);
    int max_y = ((robot_y + 1) > (HEIGHT / BLOCK_SIZE)) ? (HEIGHT / BLOCK_SIZE) : (robot_y + 1);
    for (int i = min_y; i < max_y + 1; i++) {
        for (int j = min_x; j < max_x + 1; j++) {
            if (MAP[i * WIDTH / BLOCK_SIZE + j] == 'X') {
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
                    wall_loc[0] = j * BLOCK_SIZE + (BLOCK_SIZE / 2);
                    wall_loc[1] = i * BLOCK_SIZE + (BLOCK_SIZE / 2);
                    break;
                }
            }
        }
    }
    return wall_loc;
}

void updateState4Collision(state_t *state) {
    // Runner
    int collision_runner = 0;
    while (1) {
        int *wall_loc = checkWallCollision(state->runner);
        if (wall_loc[0] == -1 && wall_loc[1] == -1) {
            free(wall_loc);
            break;
        }
        collision_runner = 1;
        uvector_t dir = calcUnitVector(state->runner.x, state->runner.y, wall_loc[0], wall_loc[1]);
        state->runner.x -= 0.5 * dir.x;
        state->runner.y -= 0.5 * dir.y;
        free(wall_loc);
    }
    if (collision_runner) {
        state->runner.vel *= 0.25;
    }
    // Chaser
    int collision_chaser = 0;
    while (1) {
        int *wall_loc = checkWallCollision(state->chaser);
        if (wall_loc[0] == -1 && wall_loc[1] == -1) {
            free(wall_loc);
            break;
        }
        collision_chaser = 1;
        uvector_t dir = calcUnitVector(state->chaser.x, state->chaser.y, wall_loc[0], wall_loc[1]);
        // vector_t robot_points = vectRobot(state->chaser);
        // printf("Collision w/ tile %d, ", (wall_loc[0] - (BLOCK_SIZE / 2)) / BLOCK_SIZE);
        // printf("%d, ", (wall_loc[1] - (BLOCK_SIZE / 2)) / BLOCK_SIZE);
        // printf("movement from %.2f, %.2f to ", state->chaser.x, state->chaser.y);
        state->chaser.x -= 0.5 * dir.x;
        state->chaser.y -= 0.5 * dir.y;
        // printf("%.2f, %.2f with ", state->chaser.x, state->chaser.y);
        // // **************
        // printf("robot xs: %.2f, ", robot_points.buffer_x[0]);
        // printf("%.2f, ", robot_points.buffer_x[1]);
        // printf("%.2f ", robot_points.buffer_x[2]);
        // printf("ys: %.2f, ", robot_points.buffer_y[0]);
        // printf("%.2f, ", robot_points.buffer_y[1]);
        // printf("%.2f\n", robot_points.buffer_y[2]);
        // free(robot_points.buffer_x);
        // free(robot_points.buffer_y);
        // **************
        free(wall_loc);
    }
    if (collision_chaser) {
        state->chaser.vel *= 0.25;
    }
}
