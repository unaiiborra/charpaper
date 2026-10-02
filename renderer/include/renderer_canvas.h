#pragma once

#include <stdint.h>

typedef union {
	uint32_t u32;

	struct {
		uint8_t b, g, r, x;
	};
} xrgb8888_t;

typedef struct {
	xrgb8888_t *pixels;
	uint32_t width, height;
	uint32_t stride; /* In bytes */
} canvas_t;
