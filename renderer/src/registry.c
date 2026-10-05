#include "logs.h"
#include "monitor/register/register.h"
#include "panic.h"
#include "protocols/wlr-layer-shell-unstable-v1-client-protocol.h"
#include "renderer.h"
#include <stddef.h>
#include <string.h>
#include <wayland-client-core.h>
#include <wayland-client-protocol.h>

static void registry_global(
	void *data,
	struct wl_registry *reg,
	uint32_t name,
	const char *iface,
	uint32_t version
)
{
	wayland_renderer *renderer = data;

#define IF_IFACE_EQ(name) if (strcmp(iface, name) == 0)

	IF_IFACE_EQ(wl_output_interface.name)
	{
		monitor_register_async(renderer, reg, name, version);
		return;
	}

	IF_IFACE_EQ(wl_compositor_interface.name)
	{
		renderer->compositor = wl_registry_bind(
			reg,
			name,
			&wl_compositor_interface,
			version < 4 ? version : 4
		);
		return;
	}

	IF_IFACE_EQ(wl_shm_interface.name)
	{
		renderer->shm = wl_registry_bind(reg, name, &wl_shm_interface, 1);
		return;
	}

	IF_IFACE_EQ(zwlr_layer_shell_v1_interface.name)
	{
		renderer->layer_shell =
			wl_registry_bind(reg, name, &zwlr_layer_shell_v1_interface, 1);
		return;
	}
}

static void registry_global_remove(void *data, struct wl_registry *reg, uint32_t name)
{
	(void)data, (void)reg, (void)name;
}

static const struct wl_registry_listener REGISTRY_LISTENER = {
	.global = registry_global,
	.global_remove = registry_global_remove,
};

void renderer_registry_init(wayland_renderer *renderer)
{
	renderer->display = wl_display_connect(NULL);

	ASSERT(renderer->display, "Cannot connect to Wayland display");

	wl_registry_add_listener(
		wl_display_get_registry(renderer->display),
		&REGISTRY_LISTENER,
		renderer
	);

	wl_display_roundtrip(renderer->display);
	wl_display_roundtrip(renderer->display);

	ASSERT(renderer->display && renderer->compositor && renderer->shm && renderer->layer_shell,
	       "Renderer failed to initialize");

	LOG_TRACE("renderer initialized");
	const char **list = NULL;
	size_t count = monitor_list_aloc(renderer, &list);

	for (size_t i = 0; i < count; i++) {
		LOG_INFO("Detected monitor %s", list[i]);
	}

	monitor_list_free(renderer, list);
}
