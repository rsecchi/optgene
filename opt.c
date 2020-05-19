#include <stdlib.h>
#include <limits.h>
#include "opt.h"
#include "parse.h"
#include "cmdline.h"
#include "proc.h"


int genesize;
char *pool[POP_SIZE];
char *tpool[POP_SIZE];
double rate[POP_SIZE];
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

/* produces a mutation */
void mutategene(char *s)
{
	int a = random() % genesize;
	s[a] = (char) ((1 << (tmpl[a])) - 1) & random();
}


/* produces a crossover */
char *crossover(char *s1, char *s2)
{
	int i;
	char *out;
	int a = random() % genesize;

	out = malloc(genesize);

	for (i = 0; i < genesize; i++) {
		if (i < a)
			out[i] = s1[i];
		else
			out[i] = s2[i];
	}

	return out;

}

/* opt ends */
void end_opt(int s)
{
	printf("bye bye\n");
	exit(0);
}

/* opt optimisation */
void opt_init()
{
	int i;
	genesize = parse(script);

	for (i = 0; i < POP_SIZE; i++)
		pool[i] = randomgene();

}

/* opt algorithm */
void opt_run()
{
	int i, g1, g2, h1, h2;
	double best;
	int generation = 0;
	int randt = RAND_MAX / 20;
	FILE *pbest;

	genesize = parse(script);
	best = -3000;

	while (1) {
		generation++;

		/* Rate the population */
		for (i = 0; i < POP_SIZE; i++) {
			rate[i] = eval(pool[i]);
			if (best < rate[i]) {
				pbest = fopen("opt.best", "w");
				makeinst(pool[i], script, pbest);
				best = rate[i];
				fclose(pbest);
				printf("%lf\n", best);
			}
		}

		printf("%d) best rate: %lf\n", generation, best);

		/* Tournament selection */
		for (i = 0; i < POP_SIZE; i++) {

			/* select both gene */
			g1 = random() % POP_SIZE;
			g2 = random() % POP_SIZE;
			h1 = (rate[g1] > rate[g2]) ? g1 : g2;

			g1 = random() % POP_SIZE;
			g2 = random() % POP_SIZE;
			h2 = (rate[g1] > rate[g2]) ? g1 : g2;

			tpool[i] = crossover(pool[h1], pool[h2]);

			if (random() < randt)
				while (random() & 0x01)
					mutategene(tpool[i]);
		}

		/* Copy population */
		for (i = 0; i < POP_SIZE; i++) {
			free(pool[i]);
			pool[i] = tpool[i];
		}

	}

}
