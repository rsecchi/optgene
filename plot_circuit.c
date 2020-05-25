#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_COMP 100
#define MAX_ROWS 256

#define COLS_PS   15
#define ROWS_PS   30
#define XMIN_PS  100
#define YMIN_PS   50 

#define COLS     128
#define ROWS     160
#define XMIN      64
#define YMIN      96

FILE* fp;
char filename[] = "opt2.cir";

int cind;			// index of elem
struct {
	int legs;		// number of terminals (2=res, 3=trans)
	char type;
	int pos;
	int node[3];    // other rows
} cmp[MAX_COMP];

int level[MAX_ROWS];
int minl[MAX_ROWS], maxl[MAX_ROWS];
int lvl, max_index;
int width;

void swap(int* a, int* b)
{
	int w;
	w = *a; *a = *b; *b = w;
}


// Calculate the start and end points of horizontal lines
void set_hlines()
{
	int i, j, pos, m;

	for(i=0; i<=max_index; i++) {
		if (level[i]==0 && i!=0)
			continue;
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
	
	for(i=1; i<=max_index; i++) { // for each level except ground

		if (level[i]==0)
			continue;

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

int untangle()
{
	int i, j;
	int count = count_crossings();
	int pcount;	
	int swaps = 0;

	// exchange levels
	for(i=2; i<=max_index; i++) {

		if (level[i] == 0)
			continue;

		for(j=2; j<=max_index; j++)
			if (i!=j && level[j]!=0 && level[j]!=lvl){
				// TBD
				swap(&level[i], &level[j]);
				set_hlines();
				pcount = count_crossings();
				if (pcount < count) {
					count = pcount;
					swaps++;
				} else {
					swap(&level[i], &level[j]);
					set_hlines();
				}
			}
	}

	// exchange components
	for(i=0; i<cind; i++)
		for(j=0; j<cind; j++)
			if (i!=j){
				swap(&cmp[i].pos, &cmp[j].pos);
				set_hlines();
				pcount = count_crossings();
				if (pcount < count) {
					pcount = count; 
					swaps++;
				} else {
					swap(&cmp[i].pos, &cmp[j].pos);
					set_hlines();
				}
			}

	return swaps;
}

int compact()
{
	int i, j;

	int busy[max_index+1];
	int a, b, m, pos, k;

	for(i=0; i<=max_index; i++)
		busy[i] = -1;

	pos = 1;
	
	for(i=0; i<cind; i++) {

		k = 0;
		while(cmp[k].pos != pos) k++;

		a=lvl; b=0;
		for(j=0; j<cmp[k].legs; j++)
		{
			if (level[cmp[k].node[j]] > b)
				b = level[cmp[k].node[j]];

			if (level[cmp[k].node[j]] < a)
				a = level[cmp[k].node[j]];	
		}

		m = -1;
		for(j=a; j<b || j==a; j++)
			if (busy[j] > m) m = busy[j];

		cmp[k].pos = m + 2;
		
		for(j=a; j<b || j==a; j++)
			busy[j] = cmp[k].pos;

		pos += 2;
	}

	set_hlines();

	return pos;
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
			cmp[cind].type = 'r';
			sscanf(line, "%s %d %d\n", tname, 
					&(cmp[cind].node[0]),
					&(cmp[cind].node[1]));
			cmp[cind].node[2] = cmp[cind].node[1];
			break;

			case 'q': // transistor
			cmp[cind].legs = 3;
			cmp[cind].type = 'q';
			sscanf(line, "%s %d %d %d\n", tname, 
					&(cmp[cind].node[0]),
					&(cmp[cind].node[2]),
					&(cmp[cind].node[1]));
			break;
			case 'v': // voltage
			cmp[cind].legs = 2;
			cmp[cind].type = 'v';
			sscanf(line, "%s %d %d\n", tname, 
					&(cmp[cind].node[0]),
					&(cmp[cind].node[1]));
			break;
		
			default: continue;
		}

		cmp[cind].pos = 2*cind + 1;

		// update levels
		for(i=0; i<cmp[cind].legs; i++) {
				n = cmp[cind].node[i];
				if (n>1 && level[n]==0) {
					level[n] = lvl++;
					if (n>max_index) max_index=n;
				}
		}

		cind++;
	}
	set_hlines();

	level[1] = lvl;    //  Supply voltage
	minl[2] = 0;

}

void print_circuit()
{
	int i;
	int xc, y0, y1, ym, ye, yt, y2;
	int l0, l1, l2;
	
	printf("<< /PageSize [%d %d] /Orientation 0 >> setpagedevice\n",
		2*XMIN_PS + width*COLS_PS,
		2*YMIN_PS + lvl*ROWS_PS);

	printf("newpath\n");
	for(i=0; i<cind; i++) {
		xc = cmp[i].pos*COLS_PS+XMIN_PS;
		l0 = cmp[i].node[0];
		l1 = cmp[i].node[1];
		l2 = cmp[i].node[2];
		y0 = level[l0]*ROWS_PS+YMIN_PS;
		y1 = level[l1]*ROWS_PS+YMIN_PS;
		ym = ROWS_PS*((level[l0] + level[l1])/2) + YMIN_PS;
		ye = ym + ROWS_PS/5; 
		yt = ROWS_PS/5;

		if (y1>y0) {
			ye = ym + ROWS_PS - ROWS_PS/5;
			yt = -yt;
			swap(&y0, &y1);
		}
		if (cmp[i].legs==2) {
			// Resistor
			if (cmp[i].type=='r') {
					printf("%d %d moveto\n", xc-COLS_PS/3, ym+ROWS_PS/5);
					printf("%d %d lineto\n", xc+COLS_PS/3, ym+ROWS_PS/5);
					printf("%d %d lineto\n", xc+COLS_PS/3, ym+ROWS_PS-ROWS_PS/5);
					printf("%d %d lineto\n", xc-COLS_PS/3, ym+ROWS_PS-ROWS_PS/5);
					printf("%d %d lineto\n", xc-COLS_PS/3, ym+ROWS_PS/5);
			}

		} else {
			y2 = level[l2]*ROWS_PS+YMIN_PS;

			// Transistor: Collector Emitter Base 
			printf("%d %d moveto\n", xc-COLS_PS/2, ym+ROWS_PS/8);
			printf("%d %d lineto\n", xc-COLS_PS/2, ym+ROWS_PS-ROWS_PS/8);
			printf("%d %d moveto\n", xc-COLS_PS/2, ym+ROWS_PS/3);
			printf("%d %d lineto\n", xc, ym+ROWS_PS/5);
			printf("%d %d moveto\n", xc-COLS_PS/2, ym+ROWS_PS-ROWS_PS/3);
			printf("%d %d lineto\n", xc, ym+ROWS_PS-ROWS_PS/5);
			printf("%d %d moveto\n", xc, ye+yt);	
			printf("%d %d lineto\n", xc, ye);
			printf("%d %d lineto\n", xc-COLS_PS/3, ye);	

			// base wire
			printf("%d %d moveto\n", xc-COLS_PS/2, ym + ROWS_PS/2);
			printf("%d %d lineto\n", xc-COLS_PS, ym + ROWS_PS/2);
			printf("%d %d lineto\n", xc-COLS_PS, y2);
			printf("%d %d moveto\n", xc-COLS_PS, y2);
			printf("%d %d 1 0 360 arc\n", xc-COLS_PS, y2);
		}

		// vertical wires
		printf("%d %d moveto\n", xc, y0);
		printf("%d %d lineto\n", xc, ym+ROWS_PS-ROWS_PS/5);
		printf("%d %d moveto\n", xc, ym+ROWS_PS/5);
		printf("%d %d lineto\n", xc, y1);
		printf("%d %d 1 0 360 arc\n", xc, y1);
		printf("%d %d moveto\n", xc, y0);
		printf("%d %d 1 0 360 arc\n", xc, y0);
	}

	// horizontal wires
	for(i=0; i<=max_index; i++)
	{
		if (level[i]==0 && i!=0)
			continue;

		if (maxl[i]>0) {
			printf("%d %d moveto\n", minl[i]*COLS_PS+XMIN_PS, 
				level[i]*ROWS_PS+YMIN_PS);
			printf("%d %d lineto\n", maxl[i]*COLS_PS+XMIN_PS, 
				level[i]*ROWS_PS+YMIN_PS);	
		}
	}	

	printf("stroke\n");

	// input 
	printf("/Times-Roman findfont\n");
	printf("12 scalefont\n");
	printf("setfont\n");
	printf("newpath\n");
	printf("%d %d moveto\n", minl[1]*COLS_PS+XMIN_PS, 
				level[1]*ROWS_PS+ROWS_PS/10+YMIN_PS);
	printf("(Vcc) show\n");
	printf("%d %d moveto\n", minl[0]*COLS_PS+XMIN_PS, 
				(level[0]-1)*ROWS_PS+ROWS_PS/2+YMIN_PS);
	printf("(GND) show\n");
	printf("%d %d moveto\n", minl[2]*COLS_PS+XMIN_PS, 
				level[2]*ROWS_PS+ROWS_PS/10+YMIN_PS);
	printf("(Vin) show\n");
	printf("%d %d moveto\n", maxl[3]*COLS_PS+XMIN_PS, 
				level[3]*ROWS_PS+ROWS_PS/10+YMIN_PS);
	printf("(Vout) show\n");

	for(i=4; i<=max_index; i++) {
		if (level[i]==0)
			continue;
		printf("%d %d moveto\n", maxl[i]*COLS_PS+XMIN_PS, 
				level[i]*ROWS_PS+ROWS_PS/10+YMIN_PS);
		printf("(%d) show\n", i);
	}
}

void print_asc()
{
	int i;
	int xc, y0, y1, ym, ye, y2;
	int l0, l1, l2;
	
	printf("Version 4\n");
	printf("SHEET 1 1464 736\n");
	for(i=0; i<cind; i++) {
		xc = cmp[i].pos*COLS;
		l0 = cmp[i].node[0];
		l1 = cmp[i].node[1];
		l2 = cmp[i].node[2];
		y0 = -level[l0]*ROWS;
		y1 = -level[l1]*ROWS;
		ym = -ROWS*((level[l0] + level[l1])/2);

		ye = 0;
		if (y1<y0) {
			ye = 1;
			swap(&y0, &y1);
		}
		if (cmp[i].legs==2) {

			// Resistor
			printf("SYMBOL res %d %d M180\n", xc-16, ym-16);
			printf("SYMATTR InstName R%d\n", i);
			printf("SYMATTR Value 200\n");

			printf("WIRE %d %d %d %d\n", 
				xc, ym-112, xc, ym-128);

		} else {
			y2 = -level[l2]*ROWS;

			if (ye) 
				printf("SYMBOL npn %d %d M180\n", xc-COLS/2, ym-ROWS/5);
			else 
				printf("SYMBOL npn %d %d R0\n", xc-COLS/2, ym-96-ROWS/5);
			printf("SYMATTR InstName Q%d\n", i);
			printf("SYMATTR Value 2N3904\n");

			// base wire
			printf("WIRE %d %d %d %d\n", 
					xc-COLS/2, ym - ROWS/2, xc-COLS, ym-ROWS/2);
			printf("WIRE %d %d %d %d\n", 
					xc-COLS, ym - ROWS/2, xc-COLS, y2);
		}

		// vertical wires
		printf("WIRE %d %d %d %d\n", xc, y0, xc, ym-ROWS/5);
		printf("WIRE %d %d %d %d\n", xc, ym-ROWS+ROWS/5, xc, y1);
	}

	// horizontal wires
	for(i=0; i<=max_index; i++)
	{
		if (level[i]==0 && i!=0)
			continue;

		if (maxl[i]>0) {
			printf("WIRE %d %d %d %d\n", 
				minl[i]*COLS, -level[i]*ROWS,
				maxl[i]*COLS, -level[i]*ROWS);	
		}
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
	while(untangle());
	width = compact();
	fprintf(stderr, "%d\n", width);
	print_circuit();
}

