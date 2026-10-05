#pragma once

#include "monitor/monitor.h"
#include <wayland-client-protocol.h>

void monitor_start_frame_loop_async(wayland_renderer *renderer, monitor_data_t *monitor);
