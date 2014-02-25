#ifndef opt_h
#define opt_h
#include <signal.h>

#define POP_SIZE 1000

extern int genesize;
extern int *tmpl;
extern char *pool[POP_SIZE];

char *gene[POP_SIZE];
char *randomgene(void);
void opt_init();
void opt_run();
void end_opt(int);

#endif
