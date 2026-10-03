#pragma once

#include "monitor/shm_buffer/shm_buffer.h"
#include "renderer_canvas.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <wayland-client-protocol.h>

typedef struct {
	bool is_animated;
	canvas_drawer_t drawer;
	void *drawer_data;
} monitor_config_t;

typedef struct {
	/* geometry event (v1) */
	int32_t x, y;               /* Position in the global compositor space */
	int32_t physical_width_mm;  /* Physical width in millimetres, 0 if unknown */
	int32_t physical_height_mm; /* Physical height in millimetres, 0 if unknown */
	enum wl_output_subpixel subpixel;
	enum wl_output_transform transform;
	char *make;  /* Manufacturer, e.g. "Dell Inc." */
	char *model; /* Model, e.g. "U2720Q" */

	/* mode event (v1) */
	uint32_t flags;
	int32_t width;
	int32_t height;
	int32_t refresh;

	/* scale event (v2) */
	int32_t scale;

	/* name and description events (v4) */
	char *name;        /* Connector name, e.g. "DP-1", NULL if not sent */
	char *description; /* Human readable description, NULL if not sent */

	/* done event (v2) */
	bool done;
} monitor_info_t;

typedef struct {
	uint32_t id; /* name */

	monitor_config_t config;
	monitor_info_t info;

	struct wl_output *output;
	struct wl_surface *surface;
	struct zwlr_layer_surface_v1 *layer_surface;
	struct wl_callback *frame_cb;

	size_t width, height;

	uint64_t frame;
	uint32_t last_time_ms;
	bool has_last_time;
	double total_time;

	shm_buffer_registry_t buffers;
} monitor_data_t;

typedef struct monitor_registry {
	struct monitor_node {
		struct monitor_node *next;
		monitor_data_t monitor_data;
	} *root;
} monitor_registry_t;
