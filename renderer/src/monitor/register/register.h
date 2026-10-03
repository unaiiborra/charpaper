#include <stddef.h>
#include <stdint.h>
#include <wayland-client-protocol.h>

void monitor_register_async(struct wl_registry *reg, uint32_t name, uint32_t version);

size_t monitor_list_aloc(const char ***list);
void monitor_list_free(const char **list);
