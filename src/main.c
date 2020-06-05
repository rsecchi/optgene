#include "opt.h"

int script_fd;
char* script;
size_t scr_size;

char default_script[] = "input.opt";

static void help(char *exename)
{
	fprintf(stderr, "%s: <script>\n", exename);
	fprintf(stderr, "\n");
	fprintf(stderr,
		"<script>    takes a configuration file generate using\n");
	fprintf(stderr,
		"            <template> as first argument and evaluates it\n");
	fprintf(stderr, "\n");
	fprintf(stderr,
		"<template>  a template for the configuration to be evaluated\n");
	fprintf(stderr,
		"            %%1, %%2, ..., %%8: are used in <template> to indicate\n");
	fprintf(stderr,
		"            parameters with 1, 2, .., 8 bits respectively. If \n");
	fprintf(stderr,
		"            <template> is not specified the file 'output.opt' is used.\n");

}

void cmdline(int argc, char *argv[])
{
	char *sname;
	struct stat s;

	if (argc > 2) {
		help(argv[0]);
		exit(1);
	}

	if (argc == 1) {
		if (access(default_script, X_OK)) {
			fprintf(stderr, "%s: %s not found or executable\n",
				argv[0], default_script);
			help(argv[0]);
			exit(1);
		}
		sname = default_script;
	} else {
		if (access(argv[1], X_OK)) {
			fprintf(stderr, "%s: %s not found or executable\n",
				argv[0], argv[1]);
			exit(1);
		}
		sname = argv[1];
	}
	script_fd = open(sname, O_RDONLY);

	if (!script_fd) {
		fprintf(stderr, "%s: %s cannot be opened for reading\n",
			argv[0], sname);
		exit(1);
	}

	// memory map script
	if(fstat(script_fd, &s)<0) {
		fprintf(stderr, "%s: %s cannot access attr\n", argv[0], sname);
		exit(1);
	}

	scr_size = s.st_size;
	script = mmap(NULL, scr_size, PROT_READ, MAP_PRIVATE, script_fd, 0);

	if (script == MAP_FAILED) {
		fprintf(stderr, "%s: %s cannot map mem\n", argv[0], sname);
		exit(1);
	}

}

int main(int argc, char *argv[])
{
	fprintf(stderr, "Params in opt.h\n");
	fprintf(stderr, "population size: %d\n", POP_SIZE);
	fprintf(stderr, "recombination rate: %d%%\n", PERC_RECOMB);
	fprintf(stderr, "mutation rate: %d%%\n", PERC_MUTAT);

	struct timeval randtime;

	cmdline(argc, argv);
	signal(SIGINT, end_opt);
	signal(SIGCHLD, SIG_IGN);

	gettimeofday(&randtime, NULL);
	srand(randtime.tv_usec);

	opt_init();
	opt_run();


	return 0;
}
