#include "frame.h"
#include "logs.h"
#include "monitor/monitor.h"
#include "monitor/shm_buffer/shm_buffer.h"
#include "renderer_canvas.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <wayland-client-protocol.h>
#include <wayland-util.h>

const struct wl_callback_listener FRAME_LISTENER;

static void std_draw(void *data, const canvas_t *c, const frame_info_t *info)
{
	(void)data, (void)info;

	for (size_t i = 0; i < c->width * c->height; i++) {
		c->pixels[i].u32 = 0xff000000;
	}
}

static frame_info_t monitor_advance_clock(monitor_data_t *monitor, uint32_t time_ms)
{
	uint32_t dt = monitor->has_last_time ? time_ms - monitor->last_time_ms : 0;

	monitor->last_time_ms = time_ms;
	monitor->has_last_time = true;
	monitor->total_time += dt;

	return (frame_info_t){
		.time_ms = monitor->total_time,
		.dt_ms = dt,
		.frame = monitor->frame++,
	};
}

static shm_buffer_t *monitor_draw_frame_buffer(monitor_data_t *monitor, frame_info_t frame_info)
{
	LOG_TRACE(
		"monitor %d frame draw request %ldx%ld",
		monitor->id,
		monitor->width,
		monitor->height
	);

	if (monitor->width == 0 || monitor->height == 0) {
		return NULL;
	}

	canvas_drawer_t drawer = monitor->config.drawer ? monitor->config.drawer : std_draw;

	shm_buffer_t *buffer =
		shm_buffer_acquire_async(&monitor->buffers, monitor->width, monitor->height);

	drawer(monitor->config.drawer_data,
	       &(canvas_t){
		       .pixels = buffer->ptr,
		       .width = monitor->width,
		       .height = monitor->height,
		       .stride = monitor->width * sizeof(xrgb8888_t),
	       },
	       &frame_info);

	return buffer;
}

static void monitor_draw_frames(monitor_data_t *monitor, frame_info_t frame_info)
{
	shm_buffer_t *buffer = monitor_draw_frame_buffer(monitor, frame_info);

	if (!buffer) {
		return;
	}

	if (monitor->config.is_animated) {
		monitor->frame_cb = wl_surface_frame(monitor->surface);
		wl_callback_add_listener(monitor->frame_cb, &FRAME_LISTENER, monitor);
	}

	wl_surface_attach(monitor->surface, buffer->wl_buffer, 0, 0);
	wl_surface_damage(monitor->surface, 0, 0, monitor->width, monitor->height);
	wl_surface_commit(monitor->surface);
}

static void done(void *data, struct wl_callback *cb, uint32_t time_ms)
{
	monitor_data_t *monitor = data;

	wl_callback_destroy(cb);
	monitor->frame_cb = NULL;

	monitor_draw_frames(monitor, monitor_advance_clock(monitor, time_ms));
}

const struct wl_callback_listener FRAME_LISTENER = {.done = done};

void monitor_start_frame_loop_async(monitor_data_t *monitor)
{
	monitor->has_last_time = false;

	monitor_draw_frames(
		monitor,
		(frame_info_t){
			.time_ms = monitor->total_time,
			.dt_ms = 0,
			.frame = monitor->frame++,
		}
	);
}
