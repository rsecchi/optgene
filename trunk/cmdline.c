#include <stdio.h>

static void help()
{
	fprintf(stderr, "opt <script> <input_template>\n"); 
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

	if (argc<3)
		help();

	return 1;
}

