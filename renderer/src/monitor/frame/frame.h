#pragma once

#include "monitor/monitor.h"
#include "monitor/shm_buffer/shm_buffer.h"
#include <wayland-client-protocol.h>

shm_buffer_t *monitor_draw_frame_buffer(monitor_data_t *monitor);
void monitor_register_to_frame_updates(monitor_data_t *monitor);
