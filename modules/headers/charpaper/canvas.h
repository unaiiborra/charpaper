#pragma once

#include <charpaper/clock.h>
#include <charpaper/rgb.h>
#include <stddef.h>
#include <stdint.h>

typedef struct {
	xrgb8888_t *pixels;
	uint32_t width, height;
	uint32_t stride; /* In bytes */
} chp_canvas_t;

typedef void (*chp_canvas_drawer_t)(
	void *data,
	const chp_canvas_t *canvas,
	const chp_clockpoint_t *frame_info
);
