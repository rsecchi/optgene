#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "opt.h"
#include "parse.h"
#include "cmdline.h"
#include "proc.h"


int main(int argc, char *argv[])
{
	int i;
	struct timeval randtime;
	int result;

	cmdline(argc, argv);

	gettimeofday(&randtime, NULL);
	srand(randtime.tv_usec);

	genesize = parse(script);

	pool[0] = malloc(genesize);
	randomgene(pool[0]);

	result = eval(pool[0]);
	printf("The result of this evauation is %d\n", result);

	//makeinst(pool[0], ptemp);

	printf("total: %d  -- filesize %d\n", genesize, size);
	for (i = 0; i < genesize; i++)
		printf("%02d) bits: %d\n", i, tmpl[i]);

	return 0;
}
