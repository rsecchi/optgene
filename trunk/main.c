#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "opt.h"
#include "parse.h"
#include "cmdline.h"
#include "proc.h"


int main(int argc, char *argv[])
{
	struct timeval randtime;

	cmdline(argc, argv);
	signal(SIGINT, end_opt);

	gettimeofday(&randtime, NULL);
	srand(randtime.tv_usec);

	opt_init();
	opt_run();

	return 0;
}

