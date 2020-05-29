#ifndef opt_h
#define opt_h
#include <signal.h>
#include <stdint.h>

/* POP_SIZE must be a multiple of 4 */
#define POP_SIZE          100    
#define PERC_RECOMB        20
#define PERC_MUTAT         10


#define SEGSIZE       (POP_SIZE/4)
#define RECOMB       ((PERC_RECOMB*POP_SIZE)/100)
#define MUTAT        ((PERC_MUTAT*POP_SIZE)/100)
#define RATED		     0x01
#define SELECTED	     0x02


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

#endif
