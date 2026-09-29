#include "monitor.h"
#include "logs.h"
#include "panic.h"
#include "state.h"
#include <assert.h>
#include <stdlib.h>

static monitor_registry_t **const MONITOR_REGISTRY = &RENDERER_STATE.monitors;

monitor_data_t *monitor_register_async(struct wl_registry *reg, uint32_t name)
{
	monitor_registry_t *new_registry = calloc(1, sizeof(monitor_registry_t));

	if (*MONITOR_REGISTRY) {
		new_registry->next = *MONITOR_REGISTRY;
	}

	*MONITOR_REGISTRY = new_registry;

	new_registry->monitor_data.output = wl_registry_bind(reg, name, &wl_output_interface, 1);
	ASSERT(new_registry->monitor_data.output, "Coul not bind to monitor");

	LOG_TRACE("Monitor %d registered", name);

	return &new_registry->monitor_data;
}
