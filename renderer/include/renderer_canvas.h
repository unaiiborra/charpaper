#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
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

typedef struct {
	uint64_t time_ms; /* Miliseconds since the animation started (monotonic, pauses excluded) */
	uint64_t dt_ms;   /* Milieconds since the previous frame */
	uint64_t frame;   /* Frame counter, starting at 0 */
} frame_info_t;

typedef void (*canvas_drawer_t)(void *data, const canvas_t *canvas, const frame_info_t *frame_info);

#ifdef __cplusplus
}
#endif
