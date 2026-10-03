#pragma once

#include "renderer_canvas.h"
#include <stdbool.h>

void renderer_init(void);
void renderer_step(void);

typedef struct renderer_monitor_config {
	bool is_animated;
	canvas_drawer_t drawer;
} render_monitor_config_t;

int renderer_setup_monitor(const char *monitor, render_monitor_config_t config);
