#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>

#include "parse.h"
#include "opt.h"
#include "cmdline.h"

int testno;
FILE *testfile;
char *testname;

double eval(char *s)
{
	int status;
	char buf[256];
	int com[2];
	int rd;

	if (pipe(com)) {
		fprintf(stderr, "cannot open pipe for reading\n");
		exit(1);
	}

	/* creates a temporary file */
	testname = tempnam(getcwd(buf, 256), NULL);
	testfile = fopen(testname, "w");
	if (!testfile) {
		perror(NULL);
		exit(1);
	}

	makeinst(s, script, testfile);
	chmod(testname, S_IXUSR);
	fclose(testfile);

	if (!vfork()) {
		close(com[0]);

		dup2(com[1], STDOUT_FILENO);
		if (system(testname));
		exit(1);
	}

	close(com[1]);
	while ((rd = read(com[0], buf, 255)))
		buf[rd] = '\0';

	if (remove(testname))
		printf("could not remove it\n");
	free(testname);
	close(com[0]);

	wait(&status);
	return atof(buf);

}
