#include "daemon.h"
#include "charpaper/interface/renderer.h"
#include "charpaper/rgb.h"
#include "charpaper/scene.h"
#include "engine.h"
#include "logs.h"
#include "panic.h"
#include "wayland_renderer.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

static void scene_function(
	const chp_scene_t *scene,
	const chp_scene_t *previous,
	chp_clockpoint_t clockpoint
)
{
	(void)previous, (void)clockpoint;

	for (size_t i = 0; i < scene->rows; i++) {
		for (size_t j = 0; j < scene->cols; j++) {
			scene->graphics[i * scene->cols + j] = (i % ('z' - 'a')) + 'a';
			scene->foreground[i * scene->cols + j] = XRGB8888_BLACK;
			scene->background[i * scene->cols + j] = XRGB8888_WHITE;
		}
	}
}

static void drawer(void *data, const chp_canvas_t *canvas, const chp_clockpoint_t *clockpoint)
{
	struct chp_engine *engine = data;
	const chp_scene_t *scene = chp_engine_draw(engine, *clockpoint);

	for (size_t j = 0; j < canvas->width; j++) {
		for (size_t i = 0; i < canvas->height; i++) {
			size_t x = i / (canvas->width / scene->rows);
			size_t y = j / (canvas->width / scene->cols);

			uint8_t color = (x + y) * 20;
			canvas->pixels[i * canvas->width + j] = XRGB8888(color, color, color);
		}
	}
}

int daemon_run(int argc, const char **argv)
{
	(void)argc, (void)argv;

	LOG_INFO("charpaper started");

	struct chp_engine *engine = chp_engine_create(scene_function, 20, 20);
	struct chp_renderer *renderer = WAYLAND_RENDERER_INTERFACE.create(engine);

	if (WAYLAND_RENDERER_INTERFACE.setup_canvas(
		    renderer,
		    "DP-1",
		    (chp_canvas_config_t){
			    .drawer = drawer,
			    .is_animated = true,
		    }
	    ) < 0) {
		return 1;
	}

	while (1) {
		WAYLAND_RENDERER_INTERFACE.step(renderer);
	}

	return 0;
}
