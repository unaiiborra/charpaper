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
	struct chp_renderer *(*chp_renderer_create)(void *data);
	void (*chp_renderer_destroy)(struct chp_renderer *renderer);

	size_t (*chp_canvas_list)(struct chp_renderer *renderer, const char ***list);
	void (*chp_canvas_list_free)(struct chp_renderer *renderer, const char **list);

	int (*chp_renderer_setup_monitor)(
		struct chp_renderer *renderer,
		const char *monitor,
		chp_canvas_config_t config
	);

	void (*chp_renderer_step)(struct chp_renderer *renderer);
} chp_renderer_interface;
