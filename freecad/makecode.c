#include <stdio.h>
#include <sys/time.h>
#include <stdlib.h>
#include <strings.h>

#define CODESEEDFILE   "/var/local/codeseed.rnd"
#define SEEDLEN        16
#define BUFLEN         20
#define MAXCHARS       10

int main(int argc, char *argv[])
{
	long int seed;
	int i, l;
	char c;
	FILE *cs;
	char buffer[BUFLEN];


	if (argc!=2) {
		fprintf(stderr, "%s <char_no>\n", argv[0]);
		exit(1);
	}

	l = atoi(argv[1]);
	if (l<1 || l>MAXCHARS) {
		fprintf(stderr, "%s: charno in [1:%d] <char_no>\n", argv[0], MAXCHARS);
		exit(1);
	}
		
	bzero(buffer, BUFLEN);
	cs = fopen(CODESEEDFILE, "r");

	fgets(buffer, SEEDLEN, cs);
	seed = atoi(buffer);
	srandom(seed);
	fclose(cs);

	for(i=0; i<l; i++) {
		do {
			c = random()%('Z'-'1') + '1'; 
		} while ( c == 'W' || c == 'M' || c == 'Q' || (c<'A' && c>'9'));
		printf("%c", c);
	}

	seed = random();
	cs = fopen(CODESEEDFILE, "w");
	fprintf(cs, "%ld",seed);
}

