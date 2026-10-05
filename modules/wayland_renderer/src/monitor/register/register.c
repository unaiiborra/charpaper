#include "monitor/register/register.h"
#include "charpaper/interface/renderer.h"
#include "logs.h"
#include "monitor/monitor.h"
#include "monitor/register/output/output.h"
#include "panic.h"
#include "renderer.h"
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

void monitor_register_async(
	wayland_renderer *renderer,
	struct wl_registry *wl_registry,
	uint32_t name,
	uint32_t version
)
{
	struct monitor_node *node = calloc(1, sizeof(struct monitor_node));

	*node = (struct monitor_node){
		.monitor_data = {.id = name},
		.next = renderer->monitors.root,
	};
	renderer->monitors.root = node;

	node->monitor_data.output = wl_registry_bind(
		wl_registry,
		name,
		&wl_output_interface,
		version < 4 ? version : 4
	);

	ASSERT(node->monitor_data.output, "Coul not bind to monitor");

	monitor_register_output_info_async(&node->monitor_data);

	LOG_TRACE("Monitor %d registered", name);
}
