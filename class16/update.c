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

void updateChaserState(state_t *state, int action) {
    if (action == 1) {
        state->chaser.vel += 4.0;
        if (state->chaser.vel > MAX_VEL) {
            state->chaser.vel = MAX_VEL;
        }
    } else if (action == 2) {
        state->chaser.ang_vel += PI / 32.0;
    } else if (action == 3) {
        state->chaser.ang_vel -= PI / 32.0;
    }
    state->chaser.theta += state->chaser.ang_vel;
    state->chaser.x += state->chaser.vel * cosf(state->chaser.theta);
    state->chaser.y -= state->chaser.vel * sinf(state->chaser.theta);
    state->user_action = 0;
}

void decayRobot(state_t *state) {
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
        state->chaser.x -= 0.5 * dir.x;
        state->chaser.y -= 0.5 * dir.y;
        free(wall_loc);
    }
    if (collision_chaser) {
        state->chaser.vel *= 0.25;
    }
}
