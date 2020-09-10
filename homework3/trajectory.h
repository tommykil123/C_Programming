#ifndef _TRAJECTORYH_
#define _TRAJECTORYH_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "init_chase.h"
#include "update.h"
#include "float.h"

#define MAX_DEPTH 4
#define WORD_MAX_LEN 256

// quard search tree node
typedef struct qst_node {
    int option; // option, 0: Do nothing, 1: v += 2, 2: angv += pi/16, 3: angv -= pi/16
    int depth;
    struct qst_node *opt0;
    struct qst_node *opt1;
    struct qst_node *opt2;
    struct qst_node *opt3;
    agent_t runner;
    agent_t chaser;
} qst_node_t;

// quad search tree
typedef struct qst {
    qst_node_t *node;
} qst_t;

qst_node_t *qst_node_create(int option);
qst_t *qst_create(void);
void qst_initialize(qst_t *qst, state_t state);
void qst_node_destroy(qst_node_t *node);
void qst_destroy(qst_t *qst);
void qst_add(qst_node_t *node, int depth);
int qst_collision(agent_t runner, agent_t chaser);
void qst_update(agent_t *chaser, int option);
void qst_decay(agent_t *chaser);
void qst_update4wallCollision(agent_t *chaser);
void qst_simulate(agent_t *chaser, int option);
double qst_search_action(qst_node_t node, int *chosen_action);
double qst_distance(agent_t runner, agent_t chaser);
#endif
