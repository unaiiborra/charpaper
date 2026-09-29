#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <wayland-client-protocol.h>

typedef struct {
	struct wl_buffer *wl_buffer;
	void *ptr;
	size_t size; /* In bytes */
} shm_buffer;

typedef struct {
	bool configured;
	struct wl_output *output;
	struct wl_surface *surface;
	struct zwlr_layer_surface_v1 *layer_surface;
	shm_buffer buffer;

	bool event_received;
	size_t width, height;
} monitor_data_t;

typedef struct monitor_registry {
	struct monitor_registry *next;
	monitor_data_t monitor_data;
} monitor_registry_t;

monitor_data_t *monitor_register_async(struct wl_registry *reg, uint32_t name);
void monitor_configure_as_background_async(monitor_data_t *monitor);
void monitor_handle_received_events_async(monitor_registry_t **monitors);
