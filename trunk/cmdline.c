#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

char* tfile;
const char default_tfile[] = "input.opt";

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

int cmdline(int argc, char* argv[])
{
	if (argc<2) {
		help(argv[0]);
		exit(1);
	}

	if (access(argv[1], X_OK)) {
		fprintf(stderr, "%s: %s not found or not executable\n", argv[0], argv[1]);
		exit(1);
	}
	
	if (argc==1)
		return 0;

	if (access(argv[2], X_OK)) {
		tfile = default_tfile;
		return 0;
	}
	
	tfile = argv[2];

	return 1;
}

