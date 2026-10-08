#pragma once

#include <stdint.h>

typedef struct chp_clockpoint {
	uint64_t time_ms; /* Miliseconds since the animation started, monotonic */
	uint64_t dt_ms;   /* Milieconds since the previous frame */
	uint64_t frame;   /* Frame counter */
} chp_clockpoint_t;

#define CHP_CLOCKPOINT_NULL (struct chp_clockpoint){0}
