#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

char* tfile;
char default_tfile[] = "input.opt";

static void help(char* exename)
{
	fprintf(stderr, "%s: <script> <input_template>\n", exename); 
	fprintf(stderr, "\n"); 
	fprintf(stderr, "<script>          takes an instance of input using\n"); 
	fprintf(stderr, "                  <input_template> and produces a\n"); 
	fprintf(stderr, "                  numeric evaluation of the instance\n"); 
	fprintf(stderr, "\n"); 
	fprintf(stderr, "<input_template>  the template file to be evaluated\n"); 
	fprintf(stderr, "                  %%1, %%2, ..., %%8: are parameter to\n"); 
	fprintf(stderr, "                  evaluate with 1, 2, to 8 bits\n"); 
	

}

void cmdline(int argc, char* argv[])
{
	if (argc<2) {
		help(argv[0]);
		exit(1);
	}

	if (access(argv[1], X_OK)) {
		fprintf(stderr, "%s: %s not found or executable\n", 
			argv[0], argv[1]);
		exit(1);
	}
	
	if (argc==2) {
		if (access(default_tfile, R_OK)) {
			fprintf(stderr, "%s: %s not found or readable\n", 
				argv[0], default_tfile);
			exit(1);
		}
		tfile = default_tfile;
		return;
	}

	if (access(argv[2], R_OK)) {
		fprintf(stderr, "%s: %s not found or readable\n", 
			argv[0], argv[2]);
		exit(1);
	}
	
	tfile = argv[2];

}

