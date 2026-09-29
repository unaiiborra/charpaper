#include "logs.h"
#include "buf_writer.h"
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>

static const char *LOG_LVL_NAMES[] = {
	[LOG_LVL_TRACE] = "trace",
	[LOG_LVL_DEBUG] = "debug",
	[LOG_LVL_INFO] = "info",
	[LOG_LVL_WARN] = "warn",
	[LOG_LVL_PANIC] = "panic",
};

void log_write(log_level_t level, const char *func, const char *fmt, ...)
{
	if (level < 0 || level > LOG_LVL_PANIC) {
		return;
	}

	va_list args;
	va_start(args, fmt);

	char buf[4096];
	buf_writer_t writer = buf_writer_new(buf, sizeof buf);

	if (level != LOG_LVL_TRACE && level != LOG_LVL_PANIC) {
		buf_write(&writer, "[%s] ", LOG_LVL_NAMES[level]);
	} else {
		buf_write(&writer, "[%s] (%s) ", LOG_LVL_NAMES[level], func);
	}

	buf_vwrite(&writer, fmt, args);
	buf_write(&writer, "\n");

	fprintf((level >= LOG_LVL_WARN) ? stderr : stdout, "%s\n", buf);

	va_end(args);
}
