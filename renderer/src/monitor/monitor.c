#include "monitor.h"
#include "layer_surface/layer_surface.h"
#include "logs.h"
#include "monitor/shm_buffer/shm_buffer.h"
#include "panic.h"
#include "state.h"
#include <assert.h>
#include <stddef.h>
#include <stdlib.h>

static monitor_registry_t *const MONITOR_REGISTRY = &RENDERER_STATE.monitors;

void monitor_register_async(struct wl_registry *reg, uint32_t name, size_t min_buffer_count)
{
	struct monitor_node *node = calloc(1, sizeof(struct monitor_node));

	if (MONITOR_REGISTRY->root) {
		node->next = MONITOR_REGISTRY->root;
	}

	MONITOR_REGISTRY->root = node;

	*node = (struct monitor_node){
		.monitor_data = {
			.id = name,
			.buffers = shm_buffer_registy_new(min_buffer_count),
		},
	};

	node->monitor_data.output = wl_registry_bind(reg, name, &wl_output_interface, 1);

	ASSERT(node->monitor_data.output, "Coul not bind to monitor");

	LOG_TRACE("Monitor %d registered", name);

	monitor_configure_as_background_async(&node->monitor_data);
}
