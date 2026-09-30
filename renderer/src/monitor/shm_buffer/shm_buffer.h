#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef struct {
	struct wl_buffer *wl_buffer;
	void *ptr;
	size_t width, height;
} shm_buffer_t;

typedef struct {
	size_t registry_min;
	size_t count;

	struct shm_buffer_node {
		struct shm_buffer_node *next;
		bool free;
		shm_buffer_t buffer;
	} *root;
} shm_buffer_registry_t;

shm_buffer_t *
shm_buffer_acquire_async(shm_buffer_registry_t *registry, size_t width, size_t height);
void shm_buffer_release_async(shm_buffer_registry_t *registry, shm_buffer_t *buffer);
