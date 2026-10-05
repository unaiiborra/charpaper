#include "buf_writer.h"
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>

buf_writer_t buf_writer_new(char *buf, size_t size)
{
	return (buf_writer_t){
		.buf = buf,
		.size = size,
		.i = 0,
	};
}

int buf_vwrite(buf_writer_t *writer, const char *format, va_list arg)
{
	if (writer->i >= writer->size) {
		return -1;
	}

	size_t maxlen = writer->size - writer->i;
	int n = vsnprintf(writer->buf + writer->i, maxlen, format, arg);

	if (n < 0) {
		writer->buf[writer->i] = '\0';
		writer->i = writer->size;
		return -1;
	}

	writer->i += n;

	if (writer->i > writer->size) {
		writer->i = writer->size;
	}

	return 0;
}

int buf_write(buf_writer_t *writer, const char *format, ...)
{
	va_list arg;
	va_start(arg, format);

	int res = buf_vwrite(writer, format, arg);

	va_end(arg);

	return res;
}
