#pragma once

#include "monitor/monitor.h"
#include <charpaper/interface/renderer.h>

struct chp_renderer {
	void *data;

	struct wl_display *display;
	struct wl_compositor *compositor;
	struct zwlr_layer_shell_v1 *layer_shell;
	struct wl_shm *shm;

	monitor_registry_t monitors;
};

typedef struct chp_renderer wayland_renderer;
