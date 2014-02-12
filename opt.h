#ifndef opt_h
#define opt_h

#define POP_SIZE 100

extern int genesize;
extern int *tmpl;
extern char *pool[POP_SIZE];

char *gene[POP_SIZE];
char *randomgene(void);
void opt_init();
void opt_run();

#endif
