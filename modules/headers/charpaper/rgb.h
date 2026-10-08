#pragma once

#include <stdint.h>

#define XRGB8888_BLACK (xrgb8888_t){0}
#define XRGB8888_WHITE (xrgb8888_t){.u32 = 0xffffffff}
#define XRGB8888(R, G, B)                                                                          \
	(xrgb8888_t)                                                                               \
	{                                                                                          \
		.xrgb = {.r = R, .g = G, .b = B}                                                   \
	}

typedef union {
	uint32_t u32;

	struct {
		uint8_t b, g, r, x;
	} xrgb;
} xrgb8888_t;
