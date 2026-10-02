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
	__attribute__((aligned(4096))) xrgb8888_t *pixels;
	uint32_t width, height;
	uint32_t stride; /* In bytes */
} canvas_t;

typedef void (*canvas_drawer_t)(canvas_t);

void renderer_register_drawer(const char *monitor, canvas_drawer_t drawer);

#ifdef __cplusplus
}
#endif
