#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct stl_hdr {
	char  text[80];
	int   nfacets;
};

struct __attribute__((__packed__)) triangle {
	float normal[3];
	float v1[3];
	float v2[3];
	float v3[3];
	unsigned short c;
};

struct stl_file {
	struct stl_hdr *hdr;
	struct triangle* trg;
	int fsize;
}* sfile;
int nofiles;
FILE* outfile;


void parse_cl(int argc, char* argv[])
{
	FILE* infile;
	int i;

	if (argc<3) {
		fprintf(stderr, "usage: %s <stl_files> <out_file>\n", argv[0]);
		exit(1);
	}

	nofiles = argc-2;
	sfile = (struct stl_file*)calloc(nofiles, sizeof(struct stl_file));
		
	for(i=0; i<nofiles; i++) {

		infile = fopen(argv[i+1],"r");	
		if (!infile) {
			fprintf(stderr, "%s: %s not found\n", argv[0], argv[i+1]);
			exit(1);
		}

		fseek(infile, 0L, SEEK_END);
		sfile[i].fsize = ftell(infile);
		fseek(infile, 0L, SEEK_SET);
		sfile[i].hdr = (struct stl_hdr*) 
			calloc(sfile[i].fsize, sizeof(char));	
		if (sfile[i].hdr == NULL) {
			fprintf(stderr, "%s: error allocating memory\n", argv[0]);
			exit(1);
		}
		sfile[i].trg = (struct triangle*) (sfile[i].hdr + 1);

		fread((char*)sfile[i].hdr, sizeof(char), sfile[i].fsize, infile);
		fclose(infile);

		printf("%d bytes copied from %s\n", sfile[i].fsize, argv[i+1]);

		if ( sizeof(struct triangle)*sfile[i].hdr->nfacets +
		    sizeof(struct stl_hdr) == sfile[i].fsize )
			printf("%s received OK\n", argv[i+1]);
		else
			printf("%s not OK\n", argv[i+1]);
		
	}

	outfile = fopen(argv[argc-1], "w");
	if (outfile == NULL) {
		fprintf(stderr, "%s: Cannot open %s for writing\n", 
			argv[0], argv[argc-1]);
		exit(1);
	}
}


void join_files()
{
int i, tfacets, tmem;
struct stl_hdr out_hdr;

	tfacets = 0;
	
	for(i=0; i<nofiles; i++)
		tfacets += sfile[i].hdr->nfacets;
	
	strcpy(out_hdr.text, "Wedo mesh");
	out_hdr.nfacets = tfacets;

	fwrite(&out_hdr, 1, sizeof(struct stl_hdr), outfile);
	
	for(i=0; i<nofiles; i++)
		fwrite(sfile[i].trg, sfile[i].hdr->nfacets, 
			sizeof(struct triangle), outfile);
}

int
main(int argc, char* argv[])
{
	int i;
	parse_cl(argc, argv);
	join_files();
}


