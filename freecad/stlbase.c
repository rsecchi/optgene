#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DELTA 0.1
#define SIZE 500
#define XOFF 100
#define YOFF 300


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
	sfile->trg = (struct triangle*) (sfile->hdr + 1);

	fread((char*)sfile->hdr, sizeof(char), sfile->fsize, infile);
	fclose(infile);

	printf("%d bytes copied from %s\n", sfile->fsize, argv[1]);

	if ( sizeof(struct triangle)*sfile->hdr->nfacets +
		sizeof(struct stl_hdr) == sfile->fsize )
		printf("%s received OK: %d triangle\n", argv[1], 
			sfile->hdr->nfacets);
	else
		printf("%s not OK\n", argv[i+1]);
		

	outfile = fopen("out.ps", "w");
	if (outfile == NULL) {
		fprintf(stderr, "%s: Cannot open out.ps for writing\n", 
			argv[0]);
		exit(1);
	}
}


float xs(float x)
{
	return SIZE * (x-minx)/side + XOFF;
}

float ys(float y)
{
	return SIZE * (y-miny)/side + YOFF;
}


void
print_proj_xy(struct triangle* t)
{
	fprintf(outfile, "newpath\n");
	fprintf(outfile, "%f %f moveto\n", xs(t->v1[0]), ys(t->v1[1]));
	fprintf(outfile, "%f %f lineto\n", xs(t->v2[0]), ys(t->v2[1]));
	fprintf(outfile, "%f %f lineto\n", xs(t->v3[0]), ys(t->v3[1]));
	fprintf(outfile, "%f %f lineto\n", xs(t->v1[0]), ys(t->v1[1]));
	fprintf(outfile, "stroke\n");
}


void
base_triangles()
{
	int i;
	struct triangle* trg = sfile->trg;
	int count = 0;

	minx = maxx = trg[0].v1[0];
	miny = maxy = trg[0].v1[1];

	/* bounding box */
	for(i=0; i<sfile->hdr->nfacets; i++)
	{
		if (trg[i].v1[2] > -DELTA && trg[i].v1[2] < DELTA &&
		    trg[i].v2[2] > -DELTA && trg[i].v2[2] < DELTA &&
		    trg[i].v3[2] > -DELTA && trg[i].v3[2] < DELTA) {
			count++;

			if (maxx < trg[i].v1[0])
				maxx = trg[i].v1[0];

			if (maxy < trg[i].v1[1])
				maxy = trg[i].v1[1];

			if (minx > trg[i].v1[0])
				minx = trg[i].v1[0];

			if (miny > trg[i].v1[1])
				miny = trg[i].v1[1];


			if (maxx < trg[i].v2[0])
				maxx = trg[i].v2[0];

			if (maxy < trg[i].v2[1])
				maxy = trg[i].v2[1];

			if (minx > trg[i].v2[0])
				minx = trg[i].v2[0];

			if (miny > trg[i].v2[1])
				miny = trg[i].v2[1];


			if (maxx < trg[i].v3[0])
				maxx = trg[i].v3[0];

			if (maxy < trg[i].v3[1])
				maxy = trg[i].v3[1];

			if (minx > trg[i].v3[0])
				minx = trg[i].v3[0];

			if (miny > trg[i].v3[1])
				miny = trg[i].v3[1];
		}
	}

	if (maxx - minx > maxy - miny)
		side = maxx - minx;
	else
		side = maxy - miny;


	for(i=0; i<sfile->hdr->nfacets; i++) {
		if (trg[i].v1[2] > -DELTA && trg[i].v1[2] < DELTA &&
		    trg[i].v2[2] > -DELTA && trg[i].v2[2] < DELTA &&
		    trg[i].v3[2] > -DELTA && trg[i].v3[2] < DELTA) {
			print_proj_xy(&trg[i]);

		}
	}

	printf("%d triangles in the base\n", count);
	printf("bounding box: %f %f %f %f\n", minx, miny, maxx, maxy);

}



int
main(int argc, char* argv[])
{
	parse_cl(argc, argv);
	base_triangles();
	//join_files();
}


