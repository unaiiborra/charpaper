#define _GNU_SOURCE

#include "monitor.h"
#include "protocols/wlr-layer-shell-unstable-v1-client-protocol.h"
#include "renderer_canvas.h"
#include "state.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <sys/mman.h>
#include <unistd.h>
#include <wayland-client-protocol.h>

/* Layer surface events */

static void configure(
	void *data,
	struct zwlr_layer_surface_v1 *layer_surface,
	uint32_t serial,
	uint32_t width,
	uint32_t height
)
{
	monitor_data_t *monitor = data;

	monitor->width = width;
	monitor->height = height;
	monitor->event_received = true;

	zwlr_layer_surface_v1_ack_configure(layer_surface, serial);
}

static void closed(void *data, struct zwlr_layer_surface_v1 *ls)
{
	(void)data, (void)ls;
	// TODO: free
}

static const struct zwlr_layer_surface_v1_listener LAYER_SURFACE_LISTENER = {
	.configure = configure,
	.closed = closed,
};

void monitor_configure_as_background_async(monitor_data_t *monitor)
{
	if (!monitor->output) {
		return;
	}

	monitor->surface = wl_compositor_create_surface(RENDERER_STATE.compositor);
	monitor->layer_surface = zwlr_layer_shell_v1_get_layer_surface(
		RENDERER_STATE.layer_shell,
		monitor->surface,
		monitor->output,
		ZWLR_LAYER_SHELL_V1_LAYER_BACKGROUND,
		"charpaper"
	);

	zwlr_layer_surface_v1_set_anchor(
		monitor->layer_surface,
		ZWLR_LAYER_SURFACE_V1_ANCHOR_TOP | ZWLR_LAYER_SURFACE_V1_ANCHOR_BOTTOM |
			ZWLR_LAYER_SURFACE_V1_ANCHOR_LEFT | ZWLR_LAYER_SURFACE_V1_ANCHOR_RIGHT
	);
	zwlr_layer_surface_v1_set_size(monitor->layer_surface, 0, 0);
	zwlr_layer_surface_v1_set_exclusive_zone(monitor->layer_surface, -1);
	zwlr_layer_surface_v1_add_listener(
		monitor->layer_surface,
		&LAYER_SURFACE_LISTENER,
		monitor
	);

	wl_surface_commit(monitor->surface);

	monitor->configured = true;
}

/* Handle received events */

static void aloc_shm_buffer(shm_buffer *buffer, size_t width, size_t height)
{
	size_t stride = width * sizeof(uint32_t);
	size_t bytes = stride * height;

	int fd = memfd_create("charpaper_buffer", MFD_CLOEXEC);
	ftruncate(fd, bytes);

	void *buf_ptr = mmap(NULL, bytes, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

	struct wl_shm_pool *shm_pool = wl_shm_create_pool(RENDERER_STATE.shm, fd, bytes);
	struct wl_buffer *wl_buffer = wl_shm_pool_create_buffer(
		shm_pool,
		0,
		width,
		height,
		stride,
		WL_SHM_FORMAT_XRGB8888
	);

	wl_shm_pool_destroy(shm_pool);
	close(fd);

	*buffer = (shm_buffer){
		.wl_buffer = wl_buffer,
		.ptr = buf_ptr,
		.size = bytes,
	};
}

static void free_shm_buffer(shm_buffer *buffer)
{
	if (!buffer || !buffer->wl_buffer) {
		return;
	}

	wl_buffer_destroy(buffer->wl_buffer);
	munmap(buffer->ptr, buffer->size);

	*buffer = (shm_buffer){0};
}

void monitor_handle_received_events_async(monitor_registry_t **monitors)
{
	for (monitor_registry_t *registry = *monitors; registry; registry = registry->next) {
		monitor_data_t *monitor = &registry->monitor_data;

		if (!monitor->event_received) {
			continue;
		}

		if (monitor->width * monitor->height * sizeof(xrgb8888_t) != monitor->buffer.size) {
			/* buffer size changed or it is the first time configured */

			free_shm_buffer(&monitor->buffer); /* checks if it is not allocated */
			aloc_shm_buffer(&monitor->buffer, monitor->width, monitor->height);
		}

		wl_surface_attach(monitor->surface, monitor->buffer.wl_buffer, 0, 0);
		wl_surface_damage(monitor->surface, 0, 0, monitor->width, monitor->height);

		wl_surface_commit(monitor->surface);

		monitor->event_received = false;
	}
}
