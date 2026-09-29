#include "renderer.h"
#include "logs.h"
#include "monitor/monitor.h"
#include "panic.h"
#include "protocols/wlr-layer-shell-unstable-v1-client-protocol.h"
#include "state.h"
#include <string.h>
#include <wayland-client-core.h>
#include <wayland-client-protocol.h>

renderer_state RENDERER_STATE = {0}; /* Global renderer state */

static void registry_global(
	__attribute__((unused)) void *data,
	struct wl_registry *reg,
	uint32_t name,
	const char *iface,
	uint32_t version
)
{
#define IF_IFACE_EQ(name) if (strcmp(iface, name) == 0)

	IF_IFACE_EQ(wl_output_interface.name)
	{
		monitor_data_t *monitor = monitor_register_async(reg, name);
		monitor_configure_as_background_async(monitor);
		return;
	}

	IF_IFACE_EQ(wl_compositor_interface.name)
	{
		RENDERER_STATE.compositor = wl_registry_bind(
			reg,
			name,
			&wl_compositor_interface,
			version < 4 ? version : 4
		);
		return;
	}

	IF_IFACE_EQ(wl_shm_interface.name)
	{
		RENDERER_STATE.shm = wl_registry_bind(reg, name, &wl_shm_interface, 1);
		return;
	}

	IF_IFACE_EQ(zwlr_layer_shell_v1_interface.name)
	{
		RENDERER_STATE.layer_shell =
			wl_registry_bind(reg, name, &zwlr_layer_shell_v1_interface, 1);
		return;
	}
}

static void registry_global_remove(void *data, struct wl_registry *reg, uint32_t name)
{
	(void)data, (void)reg, (void)name;
}

static const struct wl_registry_listener registry_listener = {
	.global = registry_global,
	.global_remove = registry_global_remove,
};

void renderer_init(void)
{
	RENDERER_STATE.display = wl_display_connect(NULL);

	ASSERT(RENDERER_STATE.display, "Cannot connect to Wayland display");

	wl_registry_add_listener(
		wl_display_get_registry(RENDERER_STATE.display),
		&registry_listener,
		NULL
	);

	wl_display_roundtrip(RENDERER_STATE.display);

	ASSERT(RENDERER_STATE.display && RENDERER_STATE.compositor && RENDERER_STATE.shm &&
		       RENDERER_STATE.layer_shell,
	       "Renderer failed to initialize");

	LOG_TRACE("renderer initialized");
}

void renderer_loop(void)
{
	while (1) {
		for (monitor_registry_t *m = RENDERER_STATE.monitors; m; m = m->next) {
			monitor_data_t *monitor = &m->monitor_data;

			if (!monitor->configured) {
			}
		}

		monitor_handle_received_events_async(&RENDERER_STATE.monitors);
		wl_display_dispatch(RENDERER_STATE.display);
	}
}
