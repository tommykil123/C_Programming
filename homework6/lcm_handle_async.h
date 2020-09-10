#pragma once

#include <lcm/lcm.h>

// handle all lcm messages currently available without blocking for more messages
// returns once all currently queued messages have been handled
// Based on https://github.com/lcm-proj/lcm/blob/master/examples/c/listener-async.c#L59
void lcm_handle_async(lcm_t *lcm);
