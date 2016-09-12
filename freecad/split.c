#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DELTA 0.1
#define SIZE 500
#define XOFF 50
#define YOFF 300
#define RADIUS 2

#define xs(x)  (SIZE*((x)-minx)/side + XOFF)
#define ys(y)  (SIZE*((y)-miny)/side + YOFF)


typedef float vector[3];

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
	else
		printf("%s not OK\n", argv[i+1]);
		

	outfile = fopen("out.ps", "w");
	if (outfile == NULL) {
		fprintf(stderr, "%s: Cannot open out.ps for writing\n", 
			argv[0]);
		exit(1);
	}
}


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

int
sec_line(vector a, vector b, float radius)
{
	float t, d, l, r1, r2, rr;
	float x1, y1, x2, y2, xh, yh;
	
	/* convert everything into double */
	x1 = a[0];
	y1 = a[1];
	x2 = b[0];
	y2 = b[1];


	if (x1 == x2 && y1 == y2)
		return 0;

	/* endpoint distances^2 from center */
	r1 = x1*x1 + y1*y1; 
	r2 = x2*x2 + y2*y2;
	rr = radius*radius;	


	if ( r1 < rr || r2 < rr )
		return 1;

	/* Now, both points are out */

	/* segment size^2 */
	l = (x2-x1)*(x2-x1) + (y2-y1)*(y2-y1);

	/* scalar product center-segment */
	d = - (x2-x1)*x1 - (y2-y1)*y1;


	if (d>0 && d<l) {
		/* center-segment vector*/
		xh = (x2-x1)*d/l + x1;
		yh = (y2-y1)*d/l + y1;
		
		if ( xh*xh + yh*yh < rr )
			return 1;
	}
		
	return 0;
}


int sec_circle(triangle* t, float radius)
{

	float D, Ds, Dt;
	float x1, y1, x2, y2, x3, y3;

	x1 = t->v1[0];
	y1 = t->v1[1];
	x2 = t->v2[0];
	y2 = t->v2[1];
	x3 = t->v3[0];
	y3 = t->v3[1];

	/* Is the center inside the triangle? */
	D  = (x2-x1)*(y3-y1) - (x3-x1)*(y2-y1);
	Ds = (  -x1)*(y3-y1) - (  -y1)*(x3-x1); 
	Dt = (x2-x1)*(  -y1) - (  -x1)*(y2-y1);

	if ( D < 0) {
		D = -D;
		Ds = -Ds;
		Dt = -Dt;
	}

	if (Dt>=0 && Ds>=0 && Ds+Dt<=D)
		return 1;	

	/* Does the circle intersect on side of the triangle? */
	if (sec_line(t->v1, t->v2, radius) ||
	    sec_line(t->v1, t->v3, radius) ||
	    sec_line(t->v2, t->v3, radius))
		return 1;

	return 0;	
}

/* split_solid: 
   Separate the triangles that touch the cilinder from the others. 

 */
int split_solid(triangle* trg, int nf, int radius)
{
	triangle work;
	int last = nf;

	for(int i=0; i<nf; i++) {
		if (trg[i].v1[2] > -DELTA && trg[i].v1[2] < DELTA &&
		    trg[i].v2[2] > -DELTA && trg[i].v2[2] < DELTA &&
		    trg[i].v3[2] > -DELTA && trg[i].v3[2] < DELTA &&
		    sec_circle(&trg[i], radius)) {


			printf("tombola .... %d\n", i);

			/* Put the triangle at the end of the list */
			last--;
			memcpy(&work, &trg[i], sizeof(triangle));
			memcpy(&trg[i], &trg[last], sizeof(triangle));
			memcpy(&trg[last], &work, sizeof(triangle));

		}
	}

	return last;
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

void print_circle()
{
	fprintf(outfile, "newpath\n");
	fprintf(outfile, "0 setgray\n");
	fprintf(outfile, "1 setlinewidth\n");
	fprintf(outfile, "%f %f %f 0 360 arc\n", 
		(double)(xs(0)), (double)(ys(0)), (double)(xs(RADIUS)-xs(0)));
	fprintf(outfile, "stroke\n");
	
} 


int
main(int argc, char* argv[])
{
	line* l;
	int nl, ind;
	parse_cl(argc, argv);
	base_triangles();

	ind = split_solid(sfile->trg, sfile->hdr->nfacets, RADIUS);
	
	printf(">>>>>>>>>>> %d\n", sfile->hdr->nfacets-ind);

	nl = border(&sfile->trg[ind], sfile->hdr->nfacets-ind, &l);
	for(int i=0; i<nl; i++)
		print_line(&l[i]);

	print_circle();

	//join_files();
}


