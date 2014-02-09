#include <stdlib.h>
#include "opt.h"
#include "parse.h"
#include "cmdline.h"
#include "proc.h"


int genesize;
char *pool[POP_SIZE];
int *tmpl;


/* create a random gene from template */
char *randomgene()
{
	int i;
	char *out;
	out = malloc(genesize);
	for (i = 0; i < genesize; i++)
		out[i] = (char) ((1 << (tmpl[i])) - 1) & random();

	return out;
}


void opt_init()
{
	int i;
	genesize = parse(script);
	for (i = 0; i < POP_SIZE; i++)
		pool[i] = randomgene();

}

void opt_run()
{
	int i;
	genesize = parse(script);
	for (i = 0; i < POP_SIZE; i++)
		printf("%d) %d\n", i, eval(pool[i]));

}
