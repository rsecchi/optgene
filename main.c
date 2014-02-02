#include <stdio.h>

#include "parse.h"

FILE* ptemp;


int main(int argc, char* argv[])
{
int i;

	ptemp = fopen("test.temp","r");

	parse(ptemp);

	printf("total: %d\n", count);
	for(i=0; i<count; i++)
		printf("%02d) bits: %d\n", i, tmpl[i]);

}

