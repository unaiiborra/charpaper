#pragma once

#include "charpaper/clock.h"
#include <charpaper/scene.h>
#include <stdint.h>

typedef void (*chp_scene_drawer_t)(
	const chp_scene_t *scene,
	const chp_scene_t *previous,
	chp_clockpoint_t clockpoint
);

struct chp_engine *chp_engine_create(chp_scene_drawer_t drawer, uint32_t rows, uint32_t cols);
void chp_engine_destroy(struct chp_engine *engine);

void chp_engine_resize(struct chp_engine *engine, uint32_t rows, uint32_t cols);

const chp_scene_t *chp_engine_draw(struct chp_engine *engine, chp_clockpoint_t clockpoint);
