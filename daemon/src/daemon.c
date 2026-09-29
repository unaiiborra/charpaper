#include "daemon.h"
#include "logs.h"
#include "renderer.h"

int daemon_run(int argc, const char **argv)
{
	(void)argc, (void)argv;

	LOG_INFO("charpaper started");

	renderer_init();
	renderer_loop();

	return 0;
}
