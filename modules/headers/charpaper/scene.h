#pragma once

#include "charpaper/rgb.h"
#include <stdatomic.h>
#include <stdint.h>

typedef uint64_t chp_char_t;

typedef struct chp_scene {
	uint32_t rows, cols;
	chp_char_t *graphics;
	xrgb8888_t *foreground;
	xrgb8888_t *background;
} chp_scene_t;
