#include "trajectory.h"
#define MAX_VEL 12.0

qst_node_t *qst_node_create(int option) {
    qst_node_t *node = calloc(1, sizeof(*node)); // calloc also zeros out the memory
    node->option = option;
    return node;
}

void qst_node_destroy(qst_node_t *node) {
    if (!node) {
        return;
    }
    qst_node_destroy(node->opt0);
    qst_node_destroy(node->opt1);
    qst_node_destroy(node->opt2);
    qst_node_destroy(node->opt3);
    free(node);
}

qst_t *qst_create(void) {
    return calloc(1, sizeof(qst_t)); // calloc also zeros out the memory
}

void qst_destroy(qst_t *qst) {
    qst_node_destroy(qst->node);
    free(qst);
}

void qst_add(qst_node_t *node, int depth) {
    if (depth > MAX_DEPTH) {
        return;
    }
    node->depth = depth;
    node->opt0 = qst_node_create(0);
    node->opt1 = qst_node_create(1);
    node->opt2 = qst_node_create(2);
    node->opt3 = qst_node_create(3);
    qst_add(node->opt0, depth + 1);
    qst_add(node->opt1, depth + 1);
    qst_add(node->opt2, depth + 1);
    qst_add(node->opt3, depth + 1);
}

void qst_initialize(qst_t *qst, state_t state) {
    qst_node_t *first_node = qst_node_create(-1);
    qst->node = first_node;
    qst->node->runner = state.runner;
    qst->node->chaser = state.chaser;
}

int qst_collision(agent_t runner, agent_t chaser) {
    vector_t runner_points = vectRobot(runner);
    vector_t chaser_points = vectRobot(chaser);
    int collided = collision(runner_points, chaser_points);
    free(runner_points.buffer_x);
    free(runner_points.buffer_y);
    free(chaser_points.buffer_x);
    free(chaser_points.buffer_y);
    // printf("%d\n", collided);
    return collided;
}

void qst_update(agent_t *chaser, int option) {
    if (option == 0) {
    } else if (option == 1) {
        chaser->vel += 2.0;
        if (chaser->vel > MAX_VEL) {
            chaser->vel = MAX_VEL;
        }
    } else if (option == 2) {
        chaser->ang_vel += PI / 16.0;
    } else if (option == 3) {
        chaser->ang_vel -= PI / 16.0;
    } else {
        fprintf(stderr, "Error in option not in range 0 - 3");
        exit(1);
    }
    chaser->theta += chaser->ang_vel;
    chaser->x += chaser->vel * cosf(chaser->theta);
    chaser->y -= chaser->vel * sinf(chaser->theta);
}

void qst_decay(agent_t *chaser) {
    chaser->ang_vel *= 0.8;
}

void qst_update4wallCollision(agent_t *chaser) {
    int collision_chaser = 0;
    while (1) {
        int *wall_loc = checkWallCollision(*chaser);
        if (wall_loc[0] == -1 && wall_loc[1] == -1) {
            free(wall_loc);
            break;
        }
        collision_chaser = 1;
        uvector_t dir = calcUnitVector(chaser->x, chaser->y, wall_loc[0], wall_loc[1]);
        chaser->x -= 0.5 * dir.x;
        chaser->y -= 0.5 * dir.y;
        free(wall_loc);
    }
    if (collision_chaser) {
        chaser->vel *= 0.25;
    }
}

void qst_simulate(agent_t *chaser, int option) {
    qst_update(chaser, option);
    qst_decay(chaser);
    qst_update4wallCollision(chaser);
}

double qst_distance(agent_t runner, agent_t chaser) {
    double dist = distance(runner.x, runner.y, chaser.x, chaser.y);
    return dist;
}

double qst_search_action(qst_node_t node, int *chosen_action) {
    if (qst_collision(node.runner, node.chaser)) {
        return 0.0;
    }
    if (node.depth >= MAX_DEPTH) {
        return qst_distance(node.runner, node.chaser);
    }
    double lowest_score = DBL_MAX;
    int lowest_score_idx = -1;
    for (int i = 0; i < 4; i++) {
        // printf("%d, %d\n", node.depth, i);
        agent_t chaser_cpy = node.chaser;
        qst_simulate(&chaser_cpy, i);
        if (!qst_collision(chaser_cpy, node.runner)) {
            for (int j = 0; j < 3; j++) {
                qst_simulate(&chaser_cpy, 0);
                if (qst_collision(chaser_cpy, node.runner)) {
                    break;
                }
            }
        }
        qst_node_t *next_node;
        if (i == 0) {
            next_node = node.opt0;
        } else if (i == 1) {
            next_node = node.opt1;
        } else if (i == 2) {
            next_node = node.opt2;
        } else if (i == 3) {
            next_node = node.opt3;
        }
        next_node->runner = node.runner;
        next_node->chaser = chaser_cpy;
        double score = qst_search_action(*next_node, chosen_action);
        score += 200.0 / ((chaser_cpy.vel > 2.0) ? 2.0 : chaser_cpy.vel);
        // if (node.depth == 0) {
        //     printf("depth: %d, action: %d, score: %.2f\n", node.depth, i, score);
        // }
        if (score < lowest_score) {
            lowest_score = score;
            lowest_score_idx = i;
        }
    }
    *chosen_action = lowest_score_idx;
    // printf("depth: %d, action chosen: %d, score: %.2f\n", node.depth, *chosen_action, lowest_score);
    return lowest_score;
}
