#pragma once

#include "monitor/shm_buffer/shm_buffer.h"
#include "renderer_canvas.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <wayland-client-protocol.h>

typedef struct {
	uint32_t id; /* name */
	struct wl_output *output;
	struct wl_surface *surface;
	struct zwlr_layer_surface_v1 *layer_surface;

	shm_buffer_registry_t buffers;

	canvas_drawer_t drawer;

	struct wl_callback *frame_cb;
	bool video;

	size_t width, height;
} monitor_data_t;

typedef struct monitor_registry {
	struct monitor_node {
		struct monitor_node *next;
		monitor_data_t monitor_data;
	} *root;
} monitor_registry_t;

void monitor_register_async(struct wl_registry *reg, uint32_t name, size_t min_buffer_count);
