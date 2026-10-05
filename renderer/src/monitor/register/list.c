#include "monitor/monitor.h"
#include "panic.h"
#include "register.h"
#include "renderer.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

size_t monitor_list_aloc(wayland_renderer *renderer, const char ***list)
{
	DEBUG_ASSERT(*list == NULL, "provided list must be zero initialized");

	/* Check node count and total name string length */

	size_t node_count = 0;
	size_t len = 0;

	for (struct monitor_node *node = renderer->monitors.root; node; node = node->next) {
		if (!node->monitor_data.info.done) {
			continue;
		}

		len += strlen(node->monitor_data.info.name);
		node_count += 1;
	}

	if (node_count == 0) {
		*list = NULL;
		return 0;
	}

	/* Allocate the buffer for the list of pointers and a big area for all the strings  */

	char **buffer = malloc(node_count * sizeof(char *));
	char *string = malloc(len + node_count);

	*list = (void *)buffer;

	/* Copy the node information into the list */

	struct monitor_node *curr = renderer->monitors.root;

	size_t buf_idx = 0;
	size_t str_idx = 0;

	while (curr) {
		const char *src = curr->monitor_data.info.name;

		const size_t start = str_idx;
		buffer[buf_idx++] = &string[start]; /* Set list pointer to substring start */

		while (src[str_idx - start]) {
			string[str_idx] = src[str_idx - start];
			str_idx += 1;
		}

		string[str_idx] = '\0';
		str_idx += 1;

		curr = curr->next;
	}

	return node_count;
}

void monitor_list_free(wayland_renderer *renderer, const char **list)
{
	(void)renderer;

	if (list) {
		free((void *)*list); /* free string */
		free((void **)list); /* free buffer */
	}
}
