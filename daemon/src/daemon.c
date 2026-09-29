#include "daemon.h"
#include "logs.h"
#include <stdarg.h>

int daemon_run(int argc, const char **argv)
{
	LOG_INFO("charpaper started");
	(void)argc, (void)argv;
	return 0;
}
