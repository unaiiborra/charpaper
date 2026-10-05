#define _GNU_SOURCE

#include "shm_buffer.h"
#include "panic.h"
#include "renderer.h"
#include <assert.h>
#include <charpaper/rgb.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>
#include <wayland-client-protocol.h>

/* Private helpers */

static const struct wl_buffer_listener WL_BUFFER_LISTENER;

static void aloc_shm_buffer(
	wayland_renderer *renderer,
	struct shm_buffer_node *node,
	size_t width,
	size_t height
)
{
	size_t stride = width * sizeof(xrgb8888_t);
	size_t bytes = stride * height;

	int fd = memfd_create("charpaper_shm_buffer", MFD_CLOEXEC);
	ASSERT(fd != -1, "memfd_create failed");

	int fres = ftruncate(fd, bytes);
	ASSERT(fres != -1, "ftruncate failed");

	void *buf_ptr = mmap(NULL, bytes, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	ASSERT(buf_ptr != MAP_FAILED, "mmap failed");
	DEBUG_ASSERT((uintptr_t)buf_ptr % 4096 == 0, "unaligned pointer");

	struct wl_shm_pool *wl_shm_pool = wl_shm_create_pool(renderer->shm, fd, bytes);
	struct wl_buffer *wl_buffer = wl_shm_pool_create_buffer(
		wl_shm_pool,
		0,
		width,
		height,
		stride,
		WL_SHM_FORMAT_XRGB8888
	);

	wl_buffer_add_listener(wl_buffer, &WL_BUFFER_LISTENER, node);

	wl_shm_pool_destroy(wl_shm_pool);
	close(fd);

	node->buffer = (shm_buffer_t){
		.wl_buffer = wl_buffer,
		.ptr = buf_ptr,
		.width = width,
		.height = height,
	};
}

static void free_shm_buffer(shm_buffer_t *buffer)
{
	if (!buffer || !buffer->wl_buffer) {
		return;
	}

	wl_buffer_destroy(buffer->wl_buffer);
	munmap(buffer->ptr, buffer->width * buffer->height * sizeof(xrgb8888_t));

	*buffer = (shm_buffer_t){0};
}

static struct shm_buffer_node *alloc_node(
	wayland_renderer *renderer,
	shm_buffer_registry_t *registry,
	size_t width,
	size_t height
)
{
	struct shm_buffer_node *node = malloc(sizeof(struct shm_buffer_node));
	ASSERT(node, "malloc failed");

	*node = (struct shm_buffer_node){
		.next = registry->root,
		.registry = registry,
		.free = true,
		.buffer = {0},
	};

	aloc_shm_buffer(renderer, node, width, height);

	registry->root = node;
	registry->count++;

	return node;
}

static void free_node(struct shm_buffer_node *node)
{
	struct shm_buffer_registry *registry = node->registry;
	DEBUG_ASSERT(registry->count > 0, "free node called when node count is 0");

	struct shm_buffer_node *prev = NULL;

	for (struct shm_buffer_node *n = registry->root; true; n = n->next) {
		if (n == node) {
			if (prev) {
				prev->next = node->next;
			} else {
				/* The node is the root, move the root forward */
				registry->root = node->next;
			}

			break;
		}

		DEBUG_ASSERT(n->next, "The provided node is not present in the registry");
		prev = n;
	}

	free_shm_buffer(&node->buffer);
	free(node);
	registry->count--;
}

static void release(void *data, struct wl_buffer *wl_buffer)
{
	(void)wl_buffer;

	struct shm_buffer_node *node = data;
	const struct shm_buffer_registry *registry = node->registry;

	DEBUG_ASSERT(wl_buffer == node->buffer.wl_buffer, "wl_buffer does not match");
	DEBUG_ASSERT(!node->free, "released a buffer that was marked as free");

	node->free = true;

	if (registry->count > registry->registry_min) {
		free_node(node);
	}
}

static const struct wl_buffer_listener WL_BUFFER_LISTENER = {.release = release};

/* Registry constructor and destructor */

shm_buffer_registry_t shm_buffer_registy_new(size_t min_buffer_count)
{
	return (shm_buffer_registry_t){
		.registry_min = min_buffer_count,
		.root = NULL,
		.count = 0, /* Min buffer count will be allocated at the first call to acquire */
	};
}

void shm_buffer_registry_destroy(shm_buffer_registry_t *registry)
{
	struct shm_buffer_node *curr = registry->root;

	while (curr) {
		struct shm_buffer_node *next = curr->next;
		free_shm_buffer(&curr->buffer);
		free(curr);

		curr = next;
	}

	*registry = (shm_buffer_registry_t){0};
}

/* Buffer control */
shm_buffer_t *shm_buffer_acquire_async(
	wayland_renderer *renderer,
	shm_buffer_registry_t *registry,
	size_t width,
	size_t height
)
{
	while (true) {
		/* Find a free buffer in the list, reallocate if size changed */
		for (struct shm_buffer_node *n = registry->root; n; n = n->next) {
			if (n->free) {
				if (n->buffer.width != width || n->buffer.height != height) {
					/* Size changed, reallocate with correct size */
					free_shm_buffer(&n->buffer);
					aloc_shm_buffer(renderer, n, width, height);
				}

				n->free = false;
				return &n->buffer;
			}
		}

		/* No free buffer is available, create either the minimum required count (first
		 * acquire call) or generate an extra buffer */
		size_t remaining = (registry->count < registry->registry_min)
					   ? registry->registry_min - registry->count
					   : 1;

		for (size_t i = 0; i < remaining; i++) {
			alloc_node(renderer, registry, width, height);
		}
	}
}

void shm_buffer_release_async(shm_buffer_registry_t *registry, shm_buffer_t *buffer)
{
	uintptr_t buffer_pt = (uintptr_t)buffer;
	uintptr_t node_pt = buffer_pt - offsetof(struct shm_buffer_node, buffer);
	struct shm_buffer_node *node = (struct shm_buffer_node *)node_pt;

	if (registry->count <= registry->registry_min) {
		/* The buffer count is the minimum, just mark as free for use */
		node->free = true;
		return;
	}

	/* There are more buffers than the minimum required, remove the node from the list */
	free_node(node);
}
