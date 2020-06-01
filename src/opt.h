#ifndef opt_h
#define opt_h
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/mman.h>


/* POP_SIZE must be a multiple of 4 */
#define POP_SIZE         1000 
#define PERC_RECOMB        20
#define PERC_MUTAT         10
#define NT                  4

#define SEGSIZE       (POP_SIZE/NT)
#define RECOMB       ((PERC_RECOMB*POP_SIZE)/100)
#define MUTAT        ((PERC_MUTAT*POP_SIZE)/100)
#define RATED		     0x01
#define SELECTED	     0x02

struct tctx {
    struct gene* seg;
    int evals;
    int tnum;
    double time_per_eval;
};

extern double eval(char *s, struct tctx*);
extern FILE *testfile;
extern char *testname;
extern int running;

extern char *script;
extern size_t scr_size;

void cmdline(int argc, char *argv[]);

struct gene {
	char* string;
	uint32_t flags;
	double rate; 
};

extern int genesize;
extern int *tmpl;
extern struct gene pool[POP_SIZE];

char *randomgene(void);
void opt_init();
void opt_run();
void end_opt(int);

extern int size;

int parse(char *);
void makeinst(char *, char*, int);

extern pthread_mutex_t eval_mutex;

#endif
