#include <stdio.h>
#include <strings.h>
#include <stdlib.h>

//#define DX       2.000
//#define DY       1.732
#define DX       4.000
#define DY       3.4641 
#define COLS       50
#define ROWS       52

const char str_hdr[] =  "WEDO test mesh";

typedef struct
{
	char text[80];
	int nfacets;
} stl_hdr;

typedef struct __attribute__((__packed__))
{
	float normal[3];
	float v1[3];
	float v2[3];
	float v3[3];
	unsigned short c;
} triangle;

int main()
{
	stl_hdr hdr;
	triangle t1, t2;
	FILE* stl_file;
	float dx;

	sprintf(hdr.text, str_hdr);
	hdr.nfacets = ROWS*COLS*2;

	stl_file = fopen("out.stl", "w");	
	if (!stl_file) {
		fprintf(stderr, "error opening out.stl\n");
		exit(1);
	}
	fwrite(&hdr, sizeof(hdr), 1, stl_file);

	for(int j=-ROWS/2; j<ROWS/2; j++) {

		dx = (j%2)*.5;

		for(int i=-COLS/2; i<COLS/2; i++)
		{

			bzero(&t1, sizeof(t1));
			bzero(&t2, sizeof(t2));
			t1.v1[0] = DX*(i+dx);
			t1.v1[1] = DY*j;
			t1.v2[0] = DX*(i+1+dx);
			t1.v2[1] = DY*j;
			t1.v3[0] = DX*(i+0.5+dx);
			t1.v3[1] = DY*(j+1);

			t2.v1[0] = DX*(i+1.5+dx);
			t2.v1[1] = DY*(j+1);
			t2.v2[0] = DX*(i+1+dx);
			t2.v2[1] = DY*j;
			t2.v3[0] = DX*(i+0.5+dx);
			t2.v3[1] = DY*(j+1);
			fwrite(&t1, sizeof(t1), 1, stl_file);
			fwrite(&t2, sizeof(t2), 1, stl_file);
		}
	}

	return 0;
}
