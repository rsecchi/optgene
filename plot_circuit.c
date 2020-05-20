#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_COMP 100
#define MAX_ROWS 256

#define COLS     15
#define ROWS     30
#define XMIN    50
#define YMIN    100

FILE* fp;
char filename[] = "opt.cir";

int cind;			// index of elem
struct {
	int legs;		// number of terminals (2=res, 3=trans)
	int pos;
	int node[3];    // other rows
} cmp[MAX_COMP];

int level[MAX_ROWS];
int minl[MAX_ROWS], maxl[MAX_ROWS];
int lvl;

void swap(int* a, int* b)
{
	int w;
	w = *a; *a = *b; *b = w;
}


// Calculate the start and end points of horizontal lines
void set_hlines()
{
	int i, j, pos, m;

	for(i=0; i<lvl; i++) {
			minl[i] = 2*cind+1; 
			maxl[i] = 0;
	}

	for(i=0; i<cind; i++) {
		pos = cmp[i].pos;

		for(j=0; j<cmp[i].legs; j++) {
			if (j==2) pos--;
			m = cmp[i].node[j];
			if (minl[m] > pos) minl[m] = pos;
			if (maxl[m] < pos) maxl[m] = pos; 

		}
	}
	
}

int count_crossings()
{
	int i, j;
	int l0, l1, l2, lev, min, max;
	int count = 0;
	
	for(i=1; i<lvl; i++) { // for each level
		lev = level[i];
		min = minl[i];
		max = maxl[i];
		for(j=0; j<cind; j++) { // for each component 

			if (!(min<cmp[j].pos && cmp[j].pos<max))
				continue;

			l0 = level[cmp[j].node[0]]; 
			l1 = level[cmp[j].node[1]]; 
			l2 = level[cmp[j].node[2]]; 
			if ( (l0>lev || l1>lev || l2>lev) &&
			     (l0<lev || l1<lev || l2<lev) )
					count++;
		}
	}

	return count;
}

void untangle()
{
	int i, j;
	int count = count_crossings();

	// exchange levels
	for(i=2; i<lvl; i++)
		for(j=2; j<lvl; j++)
			if (i!=j){
				// TBD
				swap(&level[i], &level[j]);
				set_hlines();
				if (count_crossings() < count) {
					fprintf(stderr, "pagallo\n");
					count = count_crossings(); 
				} else {
					swap(&level[i], &level[j]);
					set_hlines();
				}
			}

	// exchange components
	for(i=0; i<cind; i++)
		for(j=0; j<cind; j++)
			if (i!=j){
				swap(&cmp[i].pos, &cmp[j].pos);
				set_hlines();
				if (count_crossings() < count) {
					fprintf(stderr, "pallo\n");
					count = count_crossings(); 
				} else {
					swap(&cmp[i].pos, &cmp[j].pos);
					set_hlines();
				}
			}

}

// parse circuit file
void parse_circuit()
{
	char* line = NULL;
	size_t line_size = 0;
	int read;
	char c;
	char tname[64];
	int i, n;

	level[0] = 0; // Ground level at bottom
	lvl = 1;

	while((read = getline(&line, &line_size, fp)) != -1)
	{
		sscanf(line, "%c", &c);

		switch(c) {
			case 'r': // resistor 
			cmp[cind].legs = 2;
			sscanf(line, "%s %d %d\n", tname, 
					&(cmp[cind].node[0]),
					&(cmp[cind].node[1]));
			cmp[cind].node[2] = cmp[cind].node[1];
			break;

			case 'q': // transistor
			cmp[cind].legs = 3;
			sscanf(line, "%s %d %d %d\n", tname, 
					&(cmp[cind].node[0]),
					&(cmp[cind].node[2]),
					&(cmp[cind].node[1]));
			break;
		
			default: continue;
		}

		cmp[cind].pos = 2*cind + 1;

		// update levels
		for(i=0; i<cmp[cind].legs; i++) {
				n = cmp[cind].node[i];
				if (n>1 && level[n]==0)
					level[n] = lvl++;	
		}

		cind++;
	}
	set_hlines();

	level[1] = lvl++;    //  Supply voltage
	minl[2] = 0;

}

void print_circuit()
{
	int i;
	int xc, y0, y1, ym, ye, yt, y2;
	int l0, l1, l2;
	
	printf("newpath\n");
	for(i=0; i<cind; i++) {
		xc = cmp[i].pos*COLS+XMIN;
		l0 = cmp[i].node[0];
		l1 = cmp[i].node[1];
		l2 = cmp[i].node[2];
		y0 = level[l0]*ROWS+YMIN;
		y1 = level[l1]*ROWS+YMIN;
		ym = ROWS*((level[l0] + level[l1])/2) + YMIN;
		ye = ym + ROWS/5; yt = ROWS/5;

		if (y1<y0) {
			ye = ym + ROWS - ROWS/5;
			yt = -yt;
			swap(&y0, &y1);
		}
		if (cmp[i].legs==2) {
			// Resistor
			printf("%d %d moveto\n", xc-COLS/3, ym+ROWS/5);
			printf("%d %d lineto\n", xc+COLS/3, ym+ROWS/5);
			printf("%d %d lineto\n", xc+COLS/3, ym+ROWS-ROWS/5);
			printf("%d %d lineto\n", xc-COLS/3, ym+ROWS-ROWS/5);
			printf("%d %d lineto\n", xc-COLS/3, ym+ROWS/5);

		} else {
			y2 = level[l2]*ROWS+YMIN;

			// Transistor: Collector Emitter Base 
			printf("%d %d moveto\n", xc-COLS/2, ym+ROWS/8);
			printf("%d %d lineto\n", xc-COLS/2, ym+ROWS-ROWS/8);
			printf("%d %d moveto\n", xc-COLS/2, ym+ROWS/3);
			printf("%d %d lineto\n", xc, ym+ROWS/5);
			printf("%d %d moveto\n", xc-COLS/2, ym+ROWS-ROWS/3);
			printf("%d %d lineto\n", xc, ym+ROWS-ROWS/5);
			printf("%d %d moveto\n", xc, ye+yt);	
			printf("%d %d lineto\n", xc, ye);	
			printf("%d %d lineto\n", xc-COLS/3, ye);	

			// base wire
			printf("%d %d moveto\n", xc-COLS/2, ym + ROWS/2);
			printf("%d %d lineto\n", xc-COLS, ym + ROWS/2);
			printf("%d %d lineto\n", xc-COLS, y2);
			printf("%d %d moveto\n", xc-COLS, y2);
			printf("%d %d 1 0 360 arc\n", xc-COLS, y2);
		}

		// vertical wires
		printf("%d %d moveto\n", xc, y0);
		printf("%d %d lineto\n", xc, ym+ROWS/5);
		printf("%d %d moveto\n", xc, ym+ROWS-ROWS/5);
		printf("%d %d lineto\n", xc, y1);
		printf("%d %d 1 0 360 arc\n", xc, y1);
		printf("%d %d moveto\n", xc, y0);
		printf("%d %d 1 0 360 arc\n", xc, y0);
	}

	// horizontal wires
	for(i=0; i<lvl; i++)
	{
		if (maxl[i]>0) {
			printf("%d %d moveto\n", minl[i]*COLS+XMIN, level[i]*ROWS+YMIN);
			printf("%d %d lineto\n", maxl[i]*COLS+XMIN, level[i]*ROWS+YMIN);	
		}
	}	

	printf("stroke\n");

	// input 
	printf("/Times-Roman findfont\n");
	printf("12 scalefont\n");
	printf("setfont\n");
	printf("newpath\n");
	printf("%d %d moveto\n", minl[1]*COLS+XMIN, level[1]*ROWS+ROWS/10+YMIN);
	printf("(Vcc) show\n");
	printf("%d %d moveto\n", minl[0]*COLS+XMIN, (level[0]-1)*ROWS+ROWS/2+YMIN);
	printf("(GND) show\n");
	printf("%d %d moveto\n", minl[2]*COLS+XMIN, level[2]*ROWS+ROWS/10+YMIN);
	printf("(Vin) show\n");
	printf("%d %d moveto\n", maxl[3]*COLS+XMIN, level[3]*ROWS+ROWS/10+YMIN);
	printf("(Vout) show\n");

	for(i=4; i<lvl; i++) {
		printf("%d %d moveto\n", maxl[i]*COLS+XMIN, level[i]*ROWS+ROWS/10+YMIN);
		printf("(%d) show\n", i);
	}


}


int main(int argc, char* argv[])
{

	// parse command line
	
	fp = fopen(filename, "r");
	if (!fp) {
		fprintf(stderr, "Could not open file\n");
		exit(1);
	}

	parse_circuit();
	untangle();
	untangle();
	print_circuit();

}

