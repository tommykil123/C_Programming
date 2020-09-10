#include "potential_controller.h"

void potential_field(state_t *state) {
    settings_t settings = state->settings;
    double robot_r = sqrt(pow(ROBOT_LENGTH / 2.0, 2.0) + pow(ROBOT_WIDTH / 2.0, 2.0));
    double wall_r = BLOCK_SIZE / sqrt(2.0);

    double fx = 0.0;
    double fy = 0.0;

    uvector_t to_goal = calcUnitVector(state->chaser.x, state->chaser.y,
                                       state->runner.x, state->runner.y);
    double to_goal_dist = distance(state->chaser.x, state->chaser.y,
                                   state->runner.x, state->runner.y);
    double fx_runner = to_goal.x * settings.to_goal_magnitude *
                       pow(to_goal_dist, settings.to_goal_power);
    double fy_runner = to_goal.y * settings.to_goal_magnitude *
                       pow(to_goal_dist, settings.to_goal_power);
    fx += fx_runner;
    fy += fy_runner;

    int count = 0;
    for (int i = 0; i < MAP_W; i++) {
        for (int j = 0; j < MAP_H; j++) {
            if (MAP[j * MAP_W + i] == 'X') {
                count += 1;
                double wall_y = j * BLOCK_SIZE + (BLOCK_SIZE / 2);
                double wall_x = i * BLOCK_SIZE + (BLOCK_SIZE / 2);
                uvector_t to_chaser = calcUnitVector(wall_x, wall_y,
                                                     state->chaser.x, state->chaser.y);
                double to_obs_dist = distance(wall_x, wall_y, state->chaser.x,
                                              state->chaser.y) -
                                     (robot_r + wall_r);
                to_obs_dist = fmax(0.1, to_obs_dist);
                double fx_wall = to_chaser.x * settings.avoid_obs_magnitude *
                                 pow(to_obs_dist, settings.avoid_obs_power);
                double fy_wall = to_chaser.y * settings.avoid_obs_magnitude *
                                 pow(to_obs_dist, settings.avoid_obs_power);
                if (fabs(fx_wall) > 0.1 || fabs(fy_wall) > 0.1) {
                    printf("\nwall[%d,%d] fx: %.2f, fy: %.2f\n",
                           i, j, fx_wall, fy_wall);
                }
                fx += fx_wall;
                fy += fy_wall;
            }
        }
    }
    double target_theta = atan2(-fy, fx);
    double theta_error = target_theta - state->chaser.theta;
    if (theta_error > PI) {
        theta_error -= 2.0 * PI;
    } else if (theta_error < -PI) {
        theta_error += 2.0 * PI;
    }
    printf("\nrunner:   fx: %.2f, fy:%.2f\n",
           fx_runner, fy_runner);
    printf("target_theta: %.2f, chaser_theta:%.2f, theta_err:%.2f\n\n",
           target_theta * 180.0 / PI, state->chaser.theta * 180.0 / PI,
           theta_error * 180.0 / PI);
    double ang_velocity = 0.4 * theta_error;
    if (ang_velocity < -PI / 16.0) {
        ang_velocity = -PI / 16.0;
    }
    if (ang_velocity > PI / 16.0) {
        ang_velocity = PI / 16.0;
    }
    state->chaser.ang_vel = ang_velocity;
    state->chaser.vel = fmin(settings.max_vel, state->chaser.vel + 2.0);
}
