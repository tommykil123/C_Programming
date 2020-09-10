#ifndef _UPDATEH_
#define _UPDATEH_

#include "init_chase.h"
#include "collision.h"

typedef struct uvector {
    double x;
    double y;
} uvector_t;

double distance(double x0, double y0, double x1, double y1);
uvector_t calcUnitVector(double x0, double y0, double x1, double y1);
vector_t updateRobot(agent_t robot);
vector_t vectRobot(agent_t robot);
void updateChaserState(state_t *state, int action);
void decayRobot(state_t *state);
int *checkWallCollision(agent_t robot);
void updateState4Collision(state_t *state);
#endif
