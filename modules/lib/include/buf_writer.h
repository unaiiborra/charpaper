#pragma once

#include <stdarg.h>
#include <stddef.h>

typedef struct {
	char *buf;
	size_t size;
	size_t i;
} buf_writer_t;

buf_writer_t buf_writer_new(char *buf, size_t size);
int buf_vwrite(buf_writer_t *writer, const char *format, va_list arg);
int buf_write(buf_writer_t *writer, const char *format, ...);
