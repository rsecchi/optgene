#include <stdio.h>
#include <stdint.h>

struct symbol
{
	uint32_t sym;
	uint32_t bits;
	uint32_t offs;
};


int parse(FILE* fp)
{
int rc;

	while(rc=fgetc(fp)) {
		printf("%c",(char)rc);
	}


	return 0;
}



