#pragma once

#include "charpaper/rgb.h"
#include <stddef.h>
#include <stdint.h>

typedef struct {
	xrgb8888_t *pixels;
	uint32_t width, height;
	uint32_t stride; /* In bytes */
} chp_canvas_t;

typedef struct {
	uint64_t time_ms; /* Miliseconds since the animation started, monotonic */
	uint64_t dt_ms;   /* Milieconds since the previous frame */
	uint64_t frame;   /* Frame counter, starting at 0 */
} chp_frame_info_t;

typedef void (*chp_canvas_drawer_t)(
	void *data,
	const chp_canvas_t *canvas,
	const chp_frame_info_t *frame_info
);
