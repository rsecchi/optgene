#ifndef proc_h
#define proc_h

extern double eval(char *s);
extern FILE *testfile;
extern char *testname;
extern sem_t sync_eval;
extern int running;

#endif
