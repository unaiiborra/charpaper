#include "output.h"
#include "monitor/monitor.h"
#include <stdlib.h>
#include <string.h>
#include <wayland-client-protocol.h>

static void set_string(char **dst, const char *src)
{
	if (*dst != NULL) {
		free(*dst);
	}

	if (!src) {
		*dst = NULL;
		return;
	}

	*dst = malloc(strlen(src) + 1);
	strcpy(*dst, src);
}

static void geometry(
	void *data,
	struct wl_output *wl_output,
	int32_t x,
	int32_t y,
	int32_t physical_width,
	int32_t physical_height,
	int32_t subpixel,
	const char *make,
	const char *model,
	int32_t transform
)
{
	(void)wl_output;
	monitor_info_t *info = data;

	info->x = x;
	info->y = y;
	info->physical_width_mm = physical_width;
	info->physical_height_mm = physical_height;
	info->subpixel = subpixel;
	set_string(&info->make, make);
	set_string(&info->model, model);
	info->transform = transform;
}

static void mode(
	void *data,
	struct wl_output *wl_output,
	uint32_t flags,
	int32_t width,
	int32_t height,
	int32_t refresh
)
{
	(void)wl_output;
	monitor_info_t *info = data;

	info->flags = flags;
	info->width = width;
	info->height = height;
	info->refresh = refresh;
}

static void done(void *data, struct wl_output *wl_output)
{
	(void)wl_output;
	monitor_info_t *info = data;

	info->done = true;
}

static void scale(void *data, struct wl_output *wl_output, int32_t factor)
{
	(void)wl_output;
	monitor_info_t *info = data;

	info->scale = factor;
}

static void name(void *data, struct wl_output *wl_output, const char *name)
{
	(void)wl_output;
	monitor_info_t *info = data;

	set_string(&info->name, name);
}

static void description(void *data, struct wl_output *wl_output, const char *description)
{
	(void)wl_output;
	monitor_info_t *info = data;

	set_string(&info->description, description);
}

const struct wl_output_listener MONITOR_INFO_LISTENER = {
	.geometry = geometry,
	.mode = mode,
	.done = done,
	.scale = scale,
	.name = name,
	.description = description,
};

void monitor_register_output_info_async(monitor_data_t *monitor)
{
	wl_output_add_listener(monitor->output, &MONITOR_INFO_LISTENER, &monitor->info);
}

void monitor_free_output_info(monitor_data_t *monitor)
{
	set_string(&monitor->info.make, NULL);
	set_string(&monitor->info.model, NULL);
	set_string(&monitor->info.name, NULL);
	set_string(&monitor->info.description, NULL);
}
