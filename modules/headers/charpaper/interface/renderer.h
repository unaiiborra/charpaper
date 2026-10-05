#pragma once

#include <charpaper/canvas.h>
#include <stdbool.h>
#include <stddef.h>

struct chp_renderer;

typedef struct {
	bool is_animated;
	chp_canvas_drawer_t drawer;
	void *drawer_data;
} chp_canvas_config_t;

typedef struct {
	const char *name;

	struct chp_renderer *(*create)(void *data);
	void (*destroy)(struct chp_renderer *renderer);

	size_t (*canvas_list)(struct chp_renderer *renderer, const char ***list);
	void (*canvas_list_free)(struct chp_renderer *renderer, const char **list);

	int (*setup_canvas)(
		struct chp_renderer *renderer,
		const char *monitor,
		chp_canvas_config_t config
	);

	void (*step)(struct chp_renderer *renderer);
} chp_renderer_interface;
