#include "renderer.h"
#include "logs.h"
#include "monitor/frame/frame.h"
#include "monitor/layer_surface/layer_surface.h"
#include "monitor/monitor.h"
#include "monitor/shm_buffer/shm_buffer.h"
#include "registry.h"
#include "state.h"
#include <string.h>
#include <wayland-client-core.h>

void renderer_init(void)
{
	renderer_registry_init();
}

void renderer_step(void)
{
	wl_display_dispatch(RENDERER_STATE.display);
}

static void config_monitor(monitor_data_t *monitor, render_monitor_config_t cfg)
{
	monitor->config.drawer = cfg.drawer;
	monitor->config.drawer_data = cfg.drawer_data;
	monitor->config.is_animated = cfg.is_animated;
}

int renderer_setup_monitor(const char *monitor_name, render_monitor_config_t config)
{
	monitor_data_t *monitor = NULL;

	for (struct monitor_node *n = RENDERER_STATE.monitors.root; n; n = n->next) {
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

	monitor_configure_as_background_async(monitor);

	wl_display_roundtrip(RENDERER_STATE.display);

	monitor_start_frame_loop_async(monitor);

	LOG_INFO("%s setup", monitor_name);

	return 0;
}
