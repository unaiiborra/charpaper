#pragma once

typedef enum {
	LOG_LVL_TRACE = 0,
	LOG_LVL_DEBUG,
	LOG_LVL_INFO,
	LOG_LVL_WARN,
	LOG_LVL_PANIC,
} log_level_t;

__attribute__((format(printf, 3, 4))) void
log_write(log_level_t level, const char *func, const char *fmt, ...);

#define LOG_TRACE(...) log_write(LOG_LVL_TRACE, __func__, __VA_ARGS__)
#define LOG_DEBUG(...) log_write(LOG_LVL_DEBUG, __func__, __VA_ARGS__)
#define LOG_INFO(...)  log_write(LOG_LVL_INFO, __func__, __VA_ARGS__)
#define LOG_WARN(...)  log_write(LOG_LVL_WARN, __func__, __VA_ARGS__)
#define LOG_PANIC(...) log_write(LOG_LVL_PANIC, __func__, __VA_ARGS__)

#ifdef NDEBUG
#undef LOG_DEBUG
#define LOG_DEBUG(...)
#endif
