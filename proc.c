#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include "parse.h"
#include "opt.h"
#include "cmdline.h"

int eval(char *s)
{
	char buf[256];
	char *test;
	int com[2];
	int rd;

	if (pipe(com)) {
		fprintf(stderr, "cannot open pipe for reading\n");
		exit(1);
	}

	test = malloc(size + genesize * 3);
	if (!vfork()) {
		close(com[0]);
		makeinst(s, script, test);

		dup2(com[1], STDOUT_FILENO);
		execvp(test, &test);
		exit(1);
	}

	close(com[1]);
	while ((rd = read(com[0], buf, 255)))
		buf[rd] = '\0';

	free(test);

	return atoi(buf);

}
