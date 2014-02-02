#include <stdop.h>

#include "parse.h"

FILE* ptemp;


int main(int argc, char* argv[])
{
int count;

	ptemp = fopen("test.temp");

	count = parse(ptemp);

}

