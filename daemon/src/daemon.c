#include "daemon.h"
#include "logs.h"
#include "renderer.h"
#include "renderer_canvas.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

static void drawer(void *data, const canvas_t *canvas, const frame_info_t *info)
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

	renderer_init();

	renderer_setup_monitor(
		"DP-1",
		(render_monitor_config_t){
			.drawer = drawer,
			.is_animated = true,
		}
	);

	while (1) {
		renderer_step();
	}

	return 0;
}
