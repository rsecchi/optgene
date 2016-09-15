#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define THRS 3.99

struct stl_hdr {
	char  text[80];
	int   nfacets;
};

typedef struct __attribute__((__packed__)) {
	float normal[3];
	float v1[3];
	float v2[3];
	float v3[3];
	unsigned short c;
} triangle;


typedef struct {
	float v1[3];
	float v2[3];
} line;

struct stl_file {
	struct stl_hdr *hdr;
	triangle* trg;
	int fsize;

}* sfile;
int nofiles;
FILE* outfile;
	
/* bounding box */
float minx, miny;
float maxx, maxy;
float side;



void parse_cl(int argc, char* argv[])
{
	FILE* infile;
	int i;

	if (argc<2) {
		fprintf(stderr, "usage: %s <stl_file>\n", argv[0]);
		exit(1);
	}

	sfile = (struct stl_file*)malloc(sizeof(struct stl_file));
		
	infile = fopen(argv[1],"r");	
	if (!infile) {
		fprintf(stderr, "%s: %s not found\n", argv[0], argv[i+1]);
		exit(1);
	}

	fseek(infile, 0L, SEEK_END);
	sfile->fsize = ftell(infile);
	fseek(infile, 0L, SEEK_SET);
	sfile->hdr = (struct stl_hdr*) 
	calloc(sfile->fsize, sizeof(char));

	if (sfile->hdr == NULL) {
		fprintf(stderr, "%s: error allocating memory\n", argv[0]);
		exit(1);
	}
	sfile->trg = (triangle*) (sfile->hdr + 1);

	fread((char*)sfile->hdr, sizeof(char), sfile->fsize, infile);
	fclose(infile);

	printf("%d bytes copied from %s\n", sfile->fsize, argv[1]);

	if ( sizeof(triangle)*sfile->hdr->nfacets +
		sizeof(struct stl_hdr) == sfile->fsize )
		printf("%s received OK: %d triangle\n", argv[1], 
			sfile->hdr->nfacets);
	else {
		fprintf(stderr, "%d in header\n", sfile->hdr->nfacets);
		fprintf(stderr, "%lu read in file\n", 
			(sfile->fsize - sizeof(struct stl_hdr))/sizeof(triangle));
		fprintf(stderr, "%s not OK\n", argv[1]);
		exit(1);
	}	
	
	outfile = fopen("/tmp/unboxed.stl", "w");
	if (outfile == NULL) {
		fprintf(stderr, "%s: Cannot open out.ps for writing\n", 
			argv[0]);
		exit(1);
	}
}

void remove_top()
{
triangle* w = sfile->trg;
int i, nf;
	nf = sfile->hdr->nfacets;
	for(i=0; i<nf; i++) 
	{
		if (w[i].v1[2] > THRS ||
		    w[i].v2[2] > THRS ||
		    w[i].v3[2] > THRS)
		{
			memcpy(&w[i], &w[nf-1], sizeof(triangle));
			nf--;
			i--;
		}
	}
	nf = sfile->hdr->nfacets;

	fwrite(sfile->hdr, sizeof(struct stl_hdr), 1, outfile);
	fwrite(sfile->trg, sizeof(triangle), nf, outfile);

}




int
main(int argc, char* argv[])
{
	parse_cl(argc, argv);
	remove_top();

}


