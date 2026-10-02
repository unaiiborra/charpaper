#include "logs.h"
#include "monitor/monitor.h"
#include "monitor/shm_buffer/shm_buffer.h"
#include "protocols/wlr-layer-shell-unstable-v1-client-protocol.h"
#include "renderer_canvas.h"
#include "state.h"
#include <stddef.h>

/* Layer surface events */

static void tmp_draw(shm_buffer_t *buf)
{
	xrgb8888_t *pxl = buf->ptr;
	for (size_t i = 0; i < buf->width * buf->height; i++) {
		pxl[i].u32 = 0xff00ff00;
	}
}

static void configure(
	void *data,
	struct zwlr_layer_surface_v1 *layer_surface,
	uint32_t serial,
	uint32_t width,
	uint32_t height
)
{
	monitor_data_t *monitor = data;

	LOG_TRACE("configure (%d) %dx%d", monitor->id, width, height);

	zwlr_layer_surface_v1_ack_configure(layer_surface, serial);

	if (monitor->width == width && monitor->height == height) {
		wl_surface_commit(monitor->surface);
		return;
	}

	/* Update width and height */
	monitor->width = width;
	monitor->height = height;

	shm_buffer_t *buffer = shm_buffer_acquire_async(&monitor->buffers, width, height);
	tmp_draw(buffer);

	wl_surface_attach(monitor->surface, buffer->wl_buffer, 0, 0);
	wl_surface_damage(monitor->surface, 0, 0, monitor->width, monitor->height);
	wl_surface_commit(monitor->surface);
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
}
