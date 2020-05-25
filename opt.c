#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <stdlib.h>
#include <limits.h>
#include <semaphore.h>
#include "opt.h"
#include "parse.h"
#include "cmdline.h"
#include "proc.h"


int genesize;

struct gene pool[POP_SIZE];
struct gene tpool[POP_SIZE];
int *tmpl;


void print_gene(char* s)
{
	int i;
	for(i=0; i<genesize; i++)
		fprintf(stderr, "%3u ",
			(uint32_t)(s[i] & ((1 << (tmpl[i])) - 1)));
}

/* create a random gene from template */
void randomize(char* s)
{
	int i;
	for (i = 0; i < genesize; i++)
		s[i] = (char) ((1 << (tmpl[i])) - 1) & random();

}

/* produces a mutation */
void mutate(char *s)
{
	int a = random() % genesize;
	s[a] = (char) ((1 << (tmpl[a])) - 1) & random();
}


/* produces a crossover */
void crossover(char *s1, char *s2, char* out)
{
	int i;
	int a = random() % genesize;

	for (i = 0; i < genesize; i++)
		out[i] = (i < a) ? s1[i]: s2[i];

}

/* opt ends */
void end_opt(int s)
{
	if (!running)
		exit(0);

	printf("STOP ============\n");
	running = 0;
}

/* opt optimisation */
void opt_init()
{
	int i;
	genesize = parse(script);

	for (i = 0; i < POP_SIZE; i++) {
		pool[i].string = malloc(genesize);
		randomize(pool[i].string);
		pool[i].flags = 0;

		tpool[i].string = malloc(genesize);
		tpool[i].flags = 0;

	}
}

/* opt algorithm */
void opt_run()
{
	int i, j, a, b, n1, n2;
	double best;
	int generation = 0;
	int pbest;
	struct gene w;

	running = 1;
	genesize = parse(script);
	best = -3000;

	while (1) {
		generation++;

		/* Rate the population */
		for (i = 0; i < POP_SIZE; i++) {

			pool[i].flags &= ~SELECTED;
			if (!(pool[i].flags & RATED)) {
				pool[i].flags |= RATED;
				pool[i].rate = eval(pool[i].string);
			}
			if (best < pool[i].rate) {
				pbest = open("opt.best", O_WRONLY);
				makeinst(pool[i].string, script, pbest);
				best = pool[i].rate;
				close(pbest);
				printf("%lf\n", best);
			}
		}

		// rank population 
		for(i=0; i<POP_SIZE-1; i++)
			for(j=i+1; j<POP_SIZE; j++)
				if (pool[j].rate>pool[i].rate) {
					w = pool[i];
					pool[i] = pool[j];
					pool[j] = w;
				}

		/* calcuate crossovers */
		for(i=0; i<RECOMB; i++) {
			a = random() % POP_SIZE;
			b = random() % POP_SIZE;
			n1 = (pool[a].rate > pool[b].rate) ? a:b;
			a = random() % POP_SIZE;
			b = random() % POP_SIZE;
			n2 = (pool[a].rate > pool[b].rate) ? a:b;			

			crossover(pool[n1].string, pool[n2].string, tpool[i].string);
			tpool[i].flags = 0;
		}

		/* update the population with crossovers */
		for(i=0; i<RECOMB; i++) {
			do {
				a = random() % POP_SIZE;
				b = random() % POP_SIZE;
				n1 = (pool[a].rate < pool[b].rate) ? a:b;
			} while(pool[n1].flags & SELECTED);
			pool[n1].flags |= SELECTED;
		}


		j = 0;
		for(i=0; i<POP_SIZE; i++) {
			if (pool[i].flags & SELECTED) {
				w = pool[i];
				pool[i] = tpool[j];
				tpool[j] = w;
				j++;
			}
		}

		/*
		for(i=0; i<POP_SIZE; i++)
		{
			fprintf(stderr, "%2d  ", i); 
			print_gene(pool[i].string);
			fprintf(stderr, " [%d] ", pool[i].flags & RATED);
			fprintf(stderr, " [%d] ", pool[i].flags & SELECTED);	
			fprintf(stderr, "%lf\n", pool[i].rate);	
		}
		*/

		for(i=0; i<MUTAT; i++) {
			n1 = random() % POP_SIZE;
			mutate(pool[n1].string);	
		}
		printf("%d) best rate: %lf\n", generation, best);


	}

}
