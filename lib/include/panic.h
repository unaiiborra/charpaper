#pragma once

void throw_panic(const char *func, const char *msg);

#define PANIC(msg) throw_panic(__func__, msg)

#ifndef NDEBUG
#define __ASSERT_PREFIX(cond)       "[assertion failed! (" #cond ")]"
#define __DEBUG_ASSERT_PREFIX(cond) "[debug assertion failed! (" #cond ")]"
#else
#define __ASSERT_PREFIX(cond)
#endif

#define ASSERT(cond, msg)                                                                          \
	do {                                                                                       \
		if (!(cond)) {                                                                     \
			PANIC(__ASSERT_PREFIX(cond) msg);                                          \
		}                                                                                  \
	} while (0)

#ifndef NDEBUG
#define DEBUG_ASSERT(cond, msg)                                                                    \
	do {                                                                                       \
		if (!(cond)) {                                                                     \
			PANIC(__DEBUG_ASSERT_PREFIX(cond) msg);                                    \
		}                                                                                  \
	} while (0)
#else
#define DEBUG_ASSERT(cond, msg)
#endif
