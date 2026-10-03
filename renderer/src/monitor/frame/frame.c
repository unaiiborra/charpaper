#include "frame.h"
#include "logs.h"
#include "monitor/monitor.h"
#include "monitor/shm_buffer/shm_buffer.h"
#include "renderer_canvas.h"
#include <stdio.h>
#include <wayland-client-protocol.h>
#include <wayland-util.h>

const struct wl_callback_listener FRAME_LISTENER;

static void std_draw(canvas_t c)
{
	for (size_t i = 0; i < c.width * c.height; i++) {
		c.pixels[i].u32 = 0xff000000;
	}
}

shm_buffer_t *monitor_draw_frame_buffer(monitor_data_t *monitor)
{
	LOG_TRACE(
		"monitor %d frame draw request %ldx%ld",
		monitor->id,
		monitor->width,
		monitor->height
	);

	canvas_drawer_t drawer = monitor->config.drawer ? monitor->config.drawer : std_draw;

	shm_buffer_t *buffer =
		shm_buffer_acquire_async(&monitor->buffers, monitor->width, monitor->height);

	drawer((canvas_t){
		.pixels = buffer->ptr,
		.width = monitor->width,
		.height = monitor->height,
		.stride = monitor->width * sizeof(xrgb8888_t),
	});

	return buffer;
}

void monitor_register_to_frame_updates(monitor_data_t *monitor)
{
	shm_buffer_t *buffer = monitor_draw_frame_buffer(monitor);

	if (monitor->config.is_animated) {
		monitor->frame_cb = wl_surface_frame(monitor->surface);
		wl_callback_add_listener(monitor->frame_cb, &FRAME_LISTENER, monitor);
	} else {
		monitor->frame_cb = NULL;
	}

	wl_surface_attach(monitor->surface, buffer->wl_buffer, 0, 0);
	wl_surface_damage(monitor->surface, 0, 0, monitor->width, monitor->height);
	wl_surface_commit(monitor->surface);
}

static void done(void *data, struct wl_callback *cb, uint32_t time_ms)
{
	(void)time_ms, (void)cb, (void)data;

	monitor_data_t *monitor = data;

	/* Callbacks are one-shot, destroy it and request a new one with the next frame */
	wl_callback_destroy(cb);

	monitor_register_to_frame_updates(monitor);
}

const struct wl_callback_listener FRAME_LISTENER = {.done = done};
