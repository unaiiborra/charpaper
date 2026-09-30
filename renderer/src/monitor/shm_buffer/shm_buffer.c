#define _GNU_SOURCE

#include "shm_buffer.h"
#include "panic.h"
#include "renderer_canvas.h"
#include "state.h"
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>
#include <wayland-client-protocol.h>

static void aloc_shm_buffer(shm_buffer_t *buffer, size_t width, size_t height)
{
	size_t stride = width * sizeof(xrgb8888_t);
	size_t bytes = stride * height;

	int fd = memfd_create("charpaper_shm_buffer", MFD_CLOEXEC);
	ASSERT(fd != -1, "memfd_create failed");

	int fres = ftruncate(fd, bytes);
	ASSERT(fres != -1, "ftruncate failed");

	void *buf_ptr = mmap(NULL, bytes, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	ASSERT(buf_ptr != MAP_FAILED, "mmap failed");

	struct wl_shm_pool *wl_shm_pool = wl_shm_create_pool(RENDERER_STATE.shm, fd, bytes);
	struct wl_buffer *wl_buffer = wl_shm_pool_create_buffer(
		wl_shm_pool,
		0,
		width,
		height,
		stride,
		WL_SHM_FORMAT_XRGB8888
	);

	wl_shm_pool_destroy(wl_shm_pool);
	close(fd);

	*buffer = (shm_buffer_t){
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

shm_buffer_t *shm_buffer_acquire_async(shm_buffer_registry_t *registry, size_t width, size_t height)
{
	/* Find a free buffer in the list, reallocate if size changed */
	for (struct shm_buffer_node *n = registry->root; n; n = n->next) {
		if (n->free) {
			if (n->buffer.width != width || n->buffer.height != height) {
				/* Size changed, reallocate with correct size */
				free_shm_buffer(&n->buffer);
				aloc_shm_buffer(&n->buffer, width, height);
			}

			n->free = false;
			return &n->buffer;
		}
	}

	/* No free buffer is available, create either the minimum required count (first acquire
	 * call) or generate an extra buffer */
	size_t remaining = (registry->count < registry->registry_min)
				   ? registry->registry_min - registry->count
				   : 1;

	for (size_t i = 0; i < remaining; i++) {
		struct shm_buffer_node *node = malloc(sizeof(struct shm_buffer_node));
		ASSERT(node, "malloc failed");

		*node = (struct shm_buffer_node){
			.next = registry->root,
			.free = true,
			.buffer = {0},
		};

		aloc_shm_buffer(&node->buffer, width, height);
		registry->root = node;
		registry->count++;
	}

	registry->root->free = false;
	return &registry->root->buffer;
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

		ASSERT(n->next, "The provided node is not present in the registry");
		prev = n;
	}

	free_shm_buffer(buffer);
	free(node);
	registry->count--;
}
