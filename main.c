#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <semaphore.h>

#include "opt.h"
#include "parse.h"
#include "cmdline.h"
#include "proc.h"


int main(int argc, char *argv[])
{
	fprintf(stderr, "Params in opt.h\n");
	fprintf(stderr, "population size: %d\n", POP_SIZE);
	fprintf(stderr, "recombination rate: %d%%\n", PERC_RECOMB);
	fprintf(stderr, "mutation rate: %d%%\n", PERC_MUTAT);


	struct timeval randtime;

	sem_init(&sync_eval, 0, 1);

	cmdline(argc, argv);
	signal(SIGINT, end_opt);

	gettimeofday(&randtime, NULL);
	srand(randtime.tv_usec);

	opt_init();
	opt_run();

	return 0;
}
