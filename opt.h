#ifndef opt_h
#define opt_h

#define POP_SIZE 3

extern int genesize;
extern int *tmpl;
extern char *pool[POP_SIZE];

char *gene[POP_SIZE];

void randomgene(char *);

#endif
