#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "opt.h"
#include "parse.h"

FILE* ptemp;


int main(int argc, char* argv[])
{
int i;
struct timeval randtime;

	gettimeofday(&randtime, NULL);
	srand(randtime.tv_usec);

	ptemp = fopen("test.temp","r");

	genesize = parse(ptemp);
	
	pool[0] = malloc(genesize);
	randomgene(pool[0]);

	makeinst(pool[0], ptemp);

	printf("total: %d  -- filesize %d\n", genesize, size);
	for(i=0; i<genesize; i++)
		printf("%02d) bits: %d\n", i, tmpl[i]);

}

