#include <stdlib.h>
#include <limits.h>
#include <values.h>
#include <sys/time.h>
#include "opt.h"
#include "parse.h"
#include "cmdline.h"
#include "proc.h"


int genesize;
char *pool[POP_SIZE];
char *tpool[POP_SIZE];
char *tstr;
double rate[POP_SIZE];
int *tmpl;


/* create a random gene from template */
char *zerogene()
{
	int i;
	char *out;
	out = malloc(genesize);
	for (i = 0; i < genesize; i++)
		out[i] = 0;

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
	for (i = 0; i < 1; i++)
		pool[i] = zerogene();

}

/* brute force algorithm */
void opt_run()
{
	double csrate, best, tt, ttp;
	int generation = 0;
	FILE *pbest;
	long long unsigned ws, ms, ks;
	struct timeval bfs, now;
	int i, hs = 0;

	genesize = parse(script);
	best = -DBL_MAX;
	tstr = malloc(genesize);

	for (i = 0; i < genesize; i++)
		hs += tmpl[i];

	ms = 1LLU << hs;
	printf("%llu %u\n", ms, sizeof(ms));

	gettimeofday(&bfs, NULL);
	ttp = 0;
	for (ws = 0; ws < ms; ws++) {
		ks = ws;
		for (i = 0; i < genesize; i++) {
			tstr[i] = (char) (ks & ((1 << tmpl[i]) - 1));
			ks = ks >> tmpl[i];
		}
		csrate = eval(tstr);
		if (best < csrate) {
			pbest = fopen("opt.best", "w");
			makeinst(tstr, script, pbest);
			best = csrate;
			fclose(pbest);
			printf("%d %lf\n", generation, best);
		}
		gettimeofday(&now, NULL);
		tt = (double) (now.tv_sec - bfs.tv_sec);
		if ( tt > ttp+60) {
			ttp = tt;
			printf("%.03lf  elapsed: %ds remaining: %.2lfh\n",
			       (double) (ws * 100) / ms, (int)tt,
			       tt / ((double) (ws) / ms) / 3600.0);
		}
	}

}
