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
int testfd;

double eval(char *s)
{
	int status = 0;
	char buf[256];
	char tempfname[] = "tempXXXXXX";
	int com[2];
	int rd;

	if (pipe(com)) {
		fprintf(stderr, "cannot open pipe for reading\n");
		exit(1);
	}

	/* creates a temporary file */
	testfd = mkstemp(tempfname);
	if (!testfd) {
		perror(NULL);
		exit(1);
	}

	// create a file to be evaluated
	makeinst(s, script, testfd);
	chmod(tempfname, S_IRUSR | S_IXUSR);
	fsync(testfd);
	close(testfd);

	if (!fork()) {
		close(com[0]);

		dup2(com[1], STDOUT_FILENO);
		if (system(tempfname)) 
			fprintf(stderr, "could not execute %s\n",
				tempfname);
		exit(0);
	}

	close(com[1]);
	wait(&status);

	while ((rd = read(com[0], buf, 255)))
		buf[rd] = '\0';

	close(com[0]);
	if (remove(tempfname))
		fprintf(stderr, "could not remove %s\n", 
			tempfname);

	return atof(buf);
}

