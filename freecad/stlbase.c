#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DELTA 0.1
#define SIZE 500
#define XOFF 50
#define YOFF 300


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


int border(triangle* t, int size, line** res)
{
	int i, j;
	line* r;
	int nl = 3*size;

	r = (line *)calloc(nl, sizeof(line));

	/* create a list of lines */
	for(int i=0; i<size; i++)
	{
		memcpy(r[3*i].v1,   t[i].v1, 3*sizeof(float));
		memcpy(r[3*i].v2,   t[i].v2, 3*sizeof(float));
		memcpy(r[3*i+1].v1, t[i].v2, 3*sizeof(float));
		memcpy(r[3*i+1].v2, t[i].v3, 3*sizeof(float));
		memcpy(r[3*i+2].v1, t[i].v1, 3*sizeof(float));
		memcpy(r[3*i+2].v2, t[i].v3, 3*sizeof(float));
	}

	for(i=0; i<nl; i++)
	{
		printf("++++ i=%d  nl=%d\n", i, nl);
		for(j=i+1; j<nl; j++)
		{
			if ((r[i].v1[0] == r[j].v1[0] && 
			     r[i].v1[1] == r[j].v1[1] &&
			     r[i].v1[2] == r[j].v1[2] &&

			     r[i].v2[0] == r[j].v2[0] &&
			     r[i].v2[1] == r[j].v2[1] &&
			     r[i].v2[2] == r[j].v2[2] ) || 

			    (r[i].v1[0] == r[j].v2[0] && 
			     r[i].v1[1] == r[j].v2[1] &&
			     r[i].v1[2] == r[j].v2[2] &&

			     r[i].v2[0] == r[j].v1[0] &&
			     r[i].v2[1] == r[j].v1[1] &&
			     r[i].v2[2] == r[j].v1[2])) 
			{
				/* The lines are the equivalent */
				/* remove them! */
				printf("%d %d congruent\n", i, j);
				printf("%d -> %d congruent\n", nl-1, i);
				printf("%d -> %d congruent\n", nl-2, j);
				
				memcpy(r[j].v1, r[nl-1].v1, 3*sizeof(float));
				memcpy(r[j].v2, r[nl-1].v2, 3*sizeof(float));
				memcpy(r[i].v1, r[nl-2].v1, 3*sizeof(float));
				memcpy(r[i].v2, r[nl-2].v2, 3*sizeof(float));
				
				nl-=2;
				i--;
				break;
			}
		}
		printf("---- i=%d  nl=%d\n\n\n", i, nl);
	}

	*res = realloc(r, nl*sizeof(line));

	return nl;
}


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
	
	outfile = fopen("out.ps", "w");
	if (outfile == NULL) {
		fprintf(stderr, "%s: Cannot open out.ps for writing\n", 
			argv[0]);
		exit(1);
	}
}



#define xs(x)  (SIZE*((x)-minx)/side + XOFF)
#define ys(y)  (SIZE*((y)-miny)/side + YOFF)


void
print_line(line* t)
{
	fprintf(outfile, "newpath\n");
	fprintf(outfile, "4 setlinewidth\n");
	fprintf(outfile, "%f %f moveto\n", xs(t->v1[0]), ys(t->v1[1]));
	fprintf(outfile, "%f %f lineto\n", xs(t->v2[0]), ys(t->v2[1]));
	fprintf(outfile, "stroke\n");
}


void
print_proj_xy(triangle* t)
{
	fprintf(outfile, "newpath\n");
	fprintf(outfile, "1 setlinewidth\n");
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
	triangle* trg = sfile->trg;
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
	line* l;
	int nl;
	parse_cl(argc, argv);
	base_triangles();
	nl = border(sfile->trg, sfile->hdr->nfacets, &l);
	for(int i=0; i<nl; i++)
		print_line(&l[i]);

	//join_files();
}


