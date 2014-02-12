#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>

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
	testtext = malloc(size + genesize * 3);
	testfile = fopen(testname, "w");
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

	remove(testname);
	free(testtext);
	free(testname);

	return atoi(buf);

}
