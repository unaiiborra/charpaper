#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef union {
	uint32_t u32;

	struct {
		uint8_t b, g, r, x;
	} xrgb;
} xrgb8888_t;

typedef struct {
	xrgb8888_t *pixels;
	uint32_t width, height;
	uint32_t stride; /* In bytes */
} canvas_t;

typedef void (*canvas_drawer_t)(canvas_t);

#ifdef __cplusplus
}
#endif
