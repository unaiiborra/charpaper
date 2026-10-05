#include "daemon.h"
#include "charpaper/interface/renderer.h"
#include "logs.h"
#include "wayland_renderer.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

static void drawer(void *data, const chp_canvas_t *canvas, const chp_frame_info_t *info)
{
	(void)data, (void)info;

	for (size_t i = 0; i < canvas->width * canvas->height; i++) {
		canvas->pixels[i].xrgb.b = (uint8_t)info->frame;
	}
}

int daemon_run(int argc, const char **argv)
{
	(void)argc, (void)argv;

	LOG_INFO("charpaper started");

	struct chp_renderer *renderer = WAYLAND_RENDERER_INTERFACE.create(NULL);

	if (WAYLAND_RENDERER_INTERFACE.setup_canvas(
		    renderer,
		    "DP-1",
		    (chp_canvas_config_t){
			    .drawer = drawer,
			    .drawer_data = NULL,
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
