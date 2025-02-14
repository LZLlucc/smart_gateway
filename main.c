#include "log/log.h"
#include "app_runner.h"
#include <string.h>

int main(int argc, char **argv)
{
	if (1 == argc) {
		log_error("at least one param");
		return -1;
	}

	if (strcmp(argv[1], "app") == 0) 
		app_runner_run();

	return 0;
}
