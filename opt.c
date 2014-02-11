#include <stdlib.h>
#include "opt.h"
#include "parse.h"
#include "cmdline.h"
#include "proc.h"


#define RANDT 0


int genesize;
char *pool[POP_SIZE];
char *tpool[POP_SIZE];
int rate[POP_SIZE];
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
char* crossover(char* s1, char* s2)
{
        int i;
        char *out;
        int a = random() % genesize;

        out = malloc(genesize);

        for(i=0; i<genesize; i++) {
                if (i<a)
                        out[i] = s1[i];
                else
                        out[i] = s2[i];
        }

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
        int best;
        int generation = 0;

        genesize = parse(script);
        //for (i = 0; i < POP_SIZE; i++)
        //      printf("%d) %d\n", i, eval(pool[i]));

        while(1) {
                generation++;

                /* Rate the population */
                best = 0;
                for (i = 0; i < POP_SIZE; i++) {
                        rate[i] = eval(pool[i]);
                        if (best<rate[i])
                                best = rate[i];
                }

                printf("%d) best rate: %d\n", generation, best);

                /* Tournament selection */
                for (i = 0; i < POP_SIZE; i++) {

                        /* select both gene */
                        g1 = random() % genesize;
                        g2 = random() % genesize;
                        h1 = (rate[g1]>rate[g2]) ? g1 : g2;

                        g1 = random() % genesize;
                        g2 = random() % genesize;
                        h2 = (rate[g1]>rate[g2]) ? g1 : g2;

                        tpool[i] = crossover(pool[h1], pool[h2]);

                        if (random() < RANDT)
                                mutategene(tpool[i]);
                }


                /* Copy population */
                for (i = 0; i < POP_SIZE; i++) {
                        free(pool[i]);
                        pool[i] = tpool[i];
                }

        }

}

