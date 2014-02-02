#include <stdlib.h>
#include "opt.h"

int genesize;
char* pool[POP_SIZE];
int* tmpl;


/* create a random gene from template */
void randomgene(char* s)
{
int i;

	for(i=0; i<genesize; i++)
		s[i] = (char)((1<<(tmpl[i]-1)-1) & random());

}


