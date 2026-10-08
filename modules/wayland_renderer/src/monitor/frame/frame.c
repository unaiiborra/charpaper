#include "frame.h"
#include "logs.h"
#include "monitor/monitor.h"
#include "monitor/shm_buffer/shm_buffer.h"
#include "renderer.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <wayland-client-protocol.h>
#include <wayland-util.h>

const struct wl_callback_listener FRAME_LISTENER;

static void std_draw(void *data, const chp_canvas_t *c, const chp_clockpoint_t *info)
{
	(void)data, (void)info;

	for (size_t i = 0; i < c->width * c->height; i++) {
		c->pixels[i].u32 = 0xff000000;
	}
}

static chp_clockpoint_t monitor_advance_clock(monitor_data_t *monitor, uint32_t time_ms)
{
	uint32_t dt = monitor->has_last_time ? time_ms - monitor->last_time_ms : 0;

	monitor->last_time_ms = time_ms;
	monitor->has_last_time = true;
	monitor->total_time += dt;

	return (chp_clockpoint_t){
		.time_ms = monitor->total_time,
		.dt_ms = dt,
		.frame = monitor->frame++,
	};
}

static shm_buffer_t *monitor_draw_frame_buffer(
	wayland_renderer *renderer,
	monitor_data_t *monitor,
	chp_clockpoint_t frame_info
)
{
	LOG_TRACE(
		"monitor %d frame %ld draw request %ldx%ld",
		monitor->id,
		frame_info.frame,
		monitor->width,
		monitor->height
	);

	if (monitor->width == 0 || monitor->height == 0) {
		return NULL;
	}

	chp_canvas_drawer_t drawer = monitor->config.drawer ? monitor->config.drawer : std_draw;

	shm_buffer_t *buffer = shm_buffer_acquire_async(
		renderer,
		&monitor->buffers,
		monitor->width,
		monitor->height
	);

	drawer(renderer->data,
	       &(chp_canvas_t){
		       .pixels = buffer->ptr,
		       .width = monitor->width,
		       .height = monitor->height,
		       .stride = monitor->width * sizeof(xrgb8888_t),
	       },
	       &frame_info);

	return buffer;
}

typedef struct {
	wayland_renderer *renderer;
	monitor_data_t *monitor;
} renderer_monitor_tuple;

static void monitor_draw_frames(
	wayland_renderer *renderer,
	monitor_data_t *monitor,
	chp_clockpoint_t frame_info
)
{
	shm_buffer_t *buffer = monitor_draw_frame_buffer(renderer, monitor, frame_info);

	if (!buffer) {
		return;
	}

	if (monitor->config.is_animated) {
		monitor->frame_cb = wl_surface_frame(monitor->surface);

		renderer_monitor_tuple *data = malloc(sizeof *data);
		data->renderer = renderer;
		data->monitor = monitor;
		wl_callback_add_listener(monitor->frame_cb, &FRAME_LISTENER, data);
	}

	wl_surface_attach(monitor->surface, buffer->wl_buffer, 0, 0);
	wl_surface_damage(monitor->surface, 0, 0, monitor->width, monitor->height);
	wl_surface_commit(monitor->surface);
}

static void done(void *data, struct wl_callback *cb, uint32_t time_ms)
{
	renderer_monitor_tuple *t = data;
	wayland_renderer *renderer = t->renderer;
	monitor_data_t *monitor = t->monitor;

	wl_callback_destroy(cb);
	monitor->frame_cb = NULL;

	monitor_draw_frames(renderer, monitor, monitor_advance_clock(monitor, time_ms));

	free(data);
}

const struct wl_callback_listener FRAME_LISTENER = {.done = done};

void monitor_start_frame_loop_async(wayland_renderer *renderer, monitor_data_t *monitor)
{
	monitor->has_last_time = false;

	monitor_draw_frames(
		renderer,
		monitor,
		(chp_clockpoint_t){
			.time_ms = monitor->total_time,
			.dt_ms = 0,
			.frame = monitor->frame++,
		}
	);
}
