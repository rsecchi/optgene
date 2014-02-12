#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>



#include "parse.h"
#include "opt.h"
#include "cmdline.h"

int testno;

int eval(char *s)
{
	char buf[256];
	char *testtext;
	FILE *testfile;
	char *testname;
	int com[2];
	int rd;

	if (pipe(com)) {
		fprintf(stderr, "cannot open pipe for reading\n");
		exit(1);
	}

	/* creates a temporary file */
	testname = tempnam(getcwd(buf, 256), NULL);
	testtext = malloc(size + genesize * 3 + 1);
	if (!testtext) {
		fprintf(stderr, "malloc failed\n");
		exit(1);
	}
	testfile = fopen(testname, "w");
	if (!testfile) {
		perror(NULL);
		exit(1);
	}
	
	makeinst(s, script, testtext);
	fprintf(testfile, "%s", testtext);
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
	free(testtext);
	free(testname);
	close(com[0]);

	return atoi(buf);

}

