#pragma once

#include "monitor/monitor.h"

typedef struct {
	struct wl_display *display;
	struct wl_compositor *compositor;
	struct zwlr_layer_shell_v1 *layer_shell;
	struct wl_shm *shm;

	monitor_registry_t *monitors;
} renderer_state;

extern renderer_state RENDERER_STATE;
