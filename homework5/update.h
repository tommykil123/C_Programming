#ifndef _UPDATEH_
#define _UPDATEH_

#include "init.h"
#include "collision.h"
#include "controller.h"
#include <string.h>

#define MAX_VEL 12.0
#define ROBOT_LENGTH (20.0 * 4.0 / 3.0)
#define ROBOT_WIDTH 20.0

typedef struct uvector {
    double x;
    double y;
} uvector_t;

double distance(double x0, double y0, double x1, double y1);
uvector_t calcUnitVector(double x0, double y0, double x1, double y1);
vector_t updateRobot(agent_t robot);
vector_t vectRobot(agent_t robot);
void refreshSimulation(state_t *state);
void refreshSelected(state_t *state);
void updateSelected(state_t *state);
void updateTune(state_t *state, char *action);
void updateRunnerState(state_t *state);
void updateChaserState(state_t *state);
void decayRobot(state_t *state);
bool checkWallCollision(state_t *state, char *choice);
void updateState4Collision(state_t *state);
#endif
