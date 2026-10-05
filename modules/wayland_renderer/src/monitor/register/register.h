#include "renderer.h"
#include <charpaper/interface/renderer.h>
#include <stddef.h>
#include <stdint.h>
#include <wayland-client-protocol.h>

void monitor_register_async(
	wayland_renderer *renderer,
	struct wl_registry *reg,
	uint32_t name,
	uint32_t version
);

size_t monitor_list_aloc(wayland_renderer *renderer, const char ***list);
void monitor_list_free(wayland_renderer *renderer, const char **list);
