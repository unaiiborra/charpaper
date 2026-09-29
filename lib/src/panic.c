#include "panic.h"
#include "logs.h"
#include <stdlib.h>

void throw_panic(const char *func, const char *msg)
{
	LOG_PANIC("%s failed! %s", func, msg);
	exit(1);
}
