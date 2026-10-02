#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef struct {
	struct wl_buffer *wl_buffer;
	void *ptr;
	size_t width, height;
} shm_buffer_t;

typedef struct shm_buffer_registry {
	size_t registry_min;
	size_t count;

	struct shm_buffer_node {
		struct shm_buffer_node *next;
		/* back pointer to allow freeing when count > registry_min */
		struct shm_buffer_registry *registry;
		bool free;
		shm_buffer_t buffer;
	} *root;
} shm_buffer_registry_t;

shm_buffer_registry_t shm_buffer_registy_new(size_t min_buffer_count);
void shm_buffer_registry_destroy(shm_buffer_registry_t *registry);

shm_buffer_t *shm_buffer_acquire_async(
	shm_buffer_registry_t *registry,
	size_t width,
	size_t height
);
void shm_buffer_release_async(shm_buffer_registry_t *registry, shm_buffer_t *buffer);
