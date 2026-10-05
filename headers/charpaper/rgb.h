#pragma once

#include <stdint.h>

typedef union {
	uint32_t u32;

	struct {
		uint8_t b, g, r, x;
	} xrgb;
} xrgb8888_t;
