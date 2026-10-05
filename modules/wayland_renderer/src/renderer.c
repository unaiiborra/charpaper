#include "renderer.h"
#include "logs.h"
#include "monitor/frame/frame.h"
#include "monitor/layer_surface/layer_surface.h"
#include "monitor/monitor.h"
#include "monitor/register/register.h"
#include "monitor/shm_buffer/shm_buffer.h"
#include "registry.h"
#include "wayland_renderer.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <wayland-client-core.h>

static wayland_renderer *renderer_create(void *data)
{
	wayland_renderer *renderer = calloc(1, sizeof(wayland_renderer));

	*renderer = (wayland_renderer){.data = data};

	renderer_registry_init(renderer);

	return renderer;
}

static void renderer_destroy(wayland_renderer *renderer)
{
	// TODO deactivate

	free(renderer);
}

static void renderer_step(wayland_renderer *renderer)
{
	wl_display_dispatch(renderer->display);
}

static void config_monitor(monitor_data_t *monitor, chp_canvas_config_t cfg)
{
	monitor->config.drawer = cfg.drawer;
	monitor->config.drawer_data = cfg.drawer_data;
	monitor->config.is_animated = cfg.is_animated;
}

static int renderer_setup_monitor(
	wayland_renderer *renderer,
	const char *monitor_name,
	chp_canvas_config_t config
)
{
	monitor_data_t *monitor = NULL;

	for (struct monitor_node *n = renderer->monitors.root; n; n = n->next) {
		if (strcmp(n->monitor_data.info.name, monitor_name) == 0) {
			monitor = &n->monitor_data;
			break;
		}
	}

	if (!monitor) {
		return -1;
	}

	config_monitor(monitor, config);

	monitor->buffers = shm_buffer_registy_new(config.is_animated ? 2 : 1);
	monitor_configure_as_background_async(renderer, monitor);
	wl_display_roundtrip(renderer->display);

	monitor_start_frame_loop_async(renderer, monitor);

	LOG_INFO("%s setup", monitor_name);

	return 0;
}

const chp_renderer_interface WAYLAND_RENDERER_INTERFACE = {
	.name = "charpaper_wayland_renderer",
	.create = renderer_create,
	.destroy = renderer_destroy,
	.canvas_list = monitor_list_aloc,
	.canvas_list_free = monitor_list_free,
	.setup_canvas = renderer_setup_monitor,
	.step = renderer_step,
};
