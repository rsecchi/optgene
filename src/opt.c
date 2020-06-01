#include "opt.h"

struct tctx thread_ctx[NT];

int genesize;

struct gene pool[POP_SIZE];
struct gene tpool[POP_SIZE];
int *tmpl;

double bst;
int generation;
pthread_t t_eval[NT];
pthread_mutex_t best_mutex;

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
int mutate(char *s)
{
	int i, count = 0;

	for(i=0; i<genesize; i++)
		if (random() % genesize == 0) {
			s[i] = (char) ((1 << (tmpl[i])) - 1) & random();
			count++;
		}

	return count;
}


/* produces a crossover */
void crossover(char *s1, char *s2, char* out)
{
	int i;

	for (i = 0; i < genesize; i++)
		out[i] = (random() % 2) ? s1[i]: s2[i];
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

void* eval_seg(void* ctxp)
{
	int i;
	int pbest;
	int evals = 0;
	struct gene* p = ((struct tctx*)ctxp)->seg;
	struct timeval start, end;

	gettimeofday(&start, NULL);
	
	for (i = 0; i < SEGSIZE; i++) {
		
		p[i].flags &= ~SELECTED;
		if (!(p[i].flags & RATED)) {
			p[i].flags |= RATED;
			p[i].rate = eval(p[i].string, (struct tctx*)ctxp);
			evals++;
		}

		pthread_mutex_lock(&eval_mutex);

		if (bst < (p[i].rate)) {
			pbest = open("opt.best", O_WRONLY);
			makeinst(p[i].string, script, pbest);
			bst = p[i].rate;
			fprintf(stderr, "new best = %lf\n", bst);
			close(pbest);
		}
		pthread_mutex_unlock(&eval_mutex);	
	}

	gettimeofday(&end, NULL);

	((struct tctx*)ctxp)->time_per_eval = 
		((double)(end.tv_sec - start.tv_sec))/evals;
	((struct tctx*)ctxp)->evals = evals;

	return ctxp;
}


/* opt algorithm */
void opt_run()
{
	int i, j, a, b, n1, n2;
	struct gene w;
	struct timeval t_start, now, prev_t; 
	struct gene* seg;

	if (!genesize)
		genesize = parse(script);

	bst = -3000;
	gettimeofday(&t_start, NULL);
	prev_t = t_start;

	pthread_mutex_init(&best_mutex, NULL);
	pthread_mutex_init(&eval_mutex, NULL);

	seg = pool;
	for(i=0; i<NT; i++) {
		thread_ctx[i].tnum = i;
		thread_ctx[i].seg = seg;
		seg += SEGSIZE;
	}

	while (1) {
		generation++;
	
		/* Rate the population */
		running = 1;
		for(i=0; i<NT; i++)
			pthread_create(&t_eval[i], NULL, eval_seg,
				(void*)(&thread_ctx[i]));
		
		for(i=0; i<NT; i++)
			pthread_join(t_eval[i], NULL);			

		running = 0;

		/* print stats */
		for(i=0; i<NT; i++)
			printf("[thread %d] %d evals, t=%4.3lf\n", 
				i, thread_ctx[i].evals,
				thread_ctx[i].time_per_eval);

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

		for(i=0; i<MUTAT; i++) {
			n1 = random() % POP_SIZE;
			if (mutate(pool[n1].string))
				pool[i].flags &= ~RATED;	
		}
		gettimeofday(&now, NULL);
		printf("%d) t= %lu   delta= %lu best rate: %lf\n", 
			generation, now.tv_sec - t_start.tv_sec, 
				now.tv_sec - prev_t.tv_sec, bst);
		prev_t = now;
	}
}
