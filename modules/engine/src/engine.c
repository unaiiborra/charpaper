#include "engine.h"
#include "charpaper/clock.h"
#include "charpaper/rgb.h"
#include "charpaper/scene.h"
#include "panic.h"
#include <stdalign.h>
#include <stdatomic.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

typedef struct chp_engine {
	uint32_t rows, cols;
	chp_scene_drawer_t drawer;

	chp_scene_t scenes[2];
	size_t current_scene;
} chp_engine_t;

static void free_scene_data(chp_scene_t *scene)
{
	DEBUG_ASSERT(
		scene->graphics && scene->foreground && scene->background,
		"cannot free unallocated data"
	);

	free(scene->graphics);

	scene->graphics = NULL;
	scene->foreground = NULL;
	scene->background = NULL;
}

static void realloc_scene_data(chp_scene_t *scene, size_t rows, size_t cols)
{
	void *arena = scene->graphics;
	size_t item_count = rows * cols;
	size_t old_item_count = scene->rows * scene->cols;

	if (scene->graphics == NULL) {
		arena =
			malloc(item_count * sizeof(chp_char_t) +
			       (item_count * sizeof(xrgb8888_t) * 2));
	} else if (item_count != old_item_count) {
		free_scene_data(scene);
		arena =
			malloc(item_count * sizeof(chp_char_t) +
			       (item_count * sizeof(xrgb8888_t) * 2));
	}

	ASSERT((uintptr_t)arena % alignof(chp_char_t) == 0, "");
	ASSERT(arena, "malloc failed");

	scene->rows = rows;
	scene->cols = cols;
	scene->graphics = arena;
	scene->foreground = (void *)(scene->graphics + item_count);
	scene->background = scene->foreground + item_count;
}

static void get_scenes(chp_engine_t *engine, chp_scene_t **scene, chp_scene_t **previous)
{
	*scene = &engine->scenes[engine->current_scene];
	*previous = &engine->scenes[engine->current_scene ^ 1];

	engine->current_scene ^= 1;
}

static void black_scene_drawer(
	const chp_scene_t *scene,
	const chp_scene_t *previous,
	chp_clockpoint_t clockpoint
)
{
	(void)previous, (void)clockpoint;

	for (size_t i = 0; i < scene->rows * scene->cols; i++) {
		scene->graphics[i] = '\0';
		scene->foreground[i] = XRGB8888_BLACK;
		scene->background[i] = XRGB8888_BLACK;
	}
}

struct chp_engine *chp_engine_create(chp_scene_drawer_t drawer, uint32_t rows, uint32_t cols)
{
	chp_engine_t *engine = malloc(sizeof(chp_engine_t));
	ASSERT(engine, "malloc failed");

	*engine = (struct chp_engine){
		.rows = rows,
		.cols = cols,
		.drawer = drawer,
		.scenes = {{0}, {0}},
		.current_scene = 0,
	};

	for (size_t i = 0; i < 2; i++) {
		realloc_scene_data(&engine->scenes[i], rows, cols);

		black_scene_drawer(&engine->scenes[i], NULL, CHP_CLOCKPOINT_NULL);
	}

	return engine;
}

void chp_engine_destroy(struct chp_engine *engine)
{
	for (size_t i = 0; i < 2; i++) {
		if (engine->scenes[i].graphics != NULL) {
			free_scene_data(&engine->scenes[i]);
		}
	}

	free(engine);
}

void chp_engine_resize(struct chp_engine *engine, uint32_t rows, uint32_t cols)
{
	engine->rows = rows;
	engine->cols = cols;
}

const chp_scene_t *chp_engine_draw(struct chp_engine *engine, chp_clockpoint_t clockpoint)
{
	chp_scene_t *scene, *previous;

	get_scenes(engine, &scene, &previous);

	realloc_scene_data(scene, engine->rows, engine->cols);

	if (engine->drawer) {
		engine->drawer(scene, previous, clockpoint);
	} else {
		black_scene_drawer(scene, previous, clockpoint);
	}

	return scene;
}
