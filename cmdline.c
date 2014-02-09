#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

FILE *script;
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
	script = fopen(sname, "r");

	if (!script) {
		fprintf(stderr, "%s: %s cannot be opened for reading\n",
			argv[0], sname);
		exit(1);
	}
}
