#include <stdio.h>

#include "opt.h"
#include "parse.h"

FILE* ptemp;


int main(int argc, char* argv[])
{
int i;

	ptemp = fopen("test.temp","r");

	genesize = parse(ptemp);
	
	pool[0] = malloc(genesize);
	randomgene(pool[0]);

	printf("total: %d  -- filesize %d\n", genesize, size);
	for(i=0; i<genesize; i++)
		printf("%02d) bits: %d\n", i, tmpl[i]);

}

