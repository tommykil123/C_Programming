#include <lcm/lcm.h>
#include "l2g_t.h"
#include <stdbool.h>
#define _GNU_SOURCE
#include <time.h>

typedef struct user_data {
    int val0;
    int val1;
    int val2;
} user_data_t;

double seconds_now(void) {
    struct timespec now;
    if (clock_gettime(CLOCK_MONOTONIC, &now)) {
        fprintf(stderr, "Retrieving system time failed.\n");
        exit(1);
    }
    return now.tv_sec + now.tv_nsec / 1000000000.0;
}

void handle_l2g(const lcm_recv_buf_t *rbuf, const char *channel,
                const l2g_t *msg, void *userdata) {
    user_data_t *user_data = userdata;
    user_data->val0 = msg->l2g[0];
    user_data->val1 = msg->l2g[1];
    user_data->val2 = msg->l2g[2];
    printf("%.2f %.2f %.2f\n", msg->l2g[0], msg->l2g[1], msg->l2g[2]);
}

int main(void) {
    // we could also pass in a string of settings for special LCM configuration
    // and that would be helpful to configure a multi-robot communication scenario
    lcm_t *lcm = lcm_create(NULL);

    // Subscribe
    user_data_t user_data;
    l2g_t_subscription_t *l2g_sub = l2g_t_subscribe(lcm, "L2G", handle_l2g, &user_data);
    // Publish
    l2g_t l2g;
    l2g.utime = 0;
    l2g.l2g[0] = 1.0;
    l2g.l2g[1] = 2.0;
    l2g.l2g[2] = 3.0;
    l2g_t_publish(lcm, "L2G", &l2g);
    double start = seconds_now();
    while (true) {
        lcm_handle_timeout(lcm, 100);
        if ((seconds_now() - start) > 0.5) {
            break;
        }
    }
    l2g_t_unsubscribe(lcm, l2g_sub);
    lcm_destroy(lcm);
    return 0;
}
