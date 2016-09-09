#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#define SIZE 300
#define OFFX 100
#define OFFY 300

typedef float vector[3];

typedef struct 
__attribute__((__packed__)) {
	float normal[3];
	float v1[3];
	float v2[3];
	float v3[3];
	unsigned short c;
} triangle;


int print_point(vector pp)
{
	fprintf(stderr, "newpath\n");
	fprintf(stderr, "%f %f 3 0 360 arc\n", pp[0]*SIZE+OFFX, pp[1]*SIZE+OFFY);
	fprintf(stderr, "stroke\n");
	
}


void print_line(vector a, vector b)
{
	fprintf(stderr, "newpath\n");
	fprintf(stderr, "%f %f moveto\n", a[0]*SIZE+OFFX, a[1]*SIZE+OFFY);
	fprintf(stderr, "%f %f lineto\n", b[0]*SIZE+OFFX, b[1]*SIZE+OFFY);
	fprintf(stderr, "stroke\n");
}

void print_triangle(triangle* t)
{
	print_line(t->v1, t->v2);
	print_line(t->v1, t->v3);
	print_line(t->v2, t->v3);
}

void print_triangle_black(triangle* t)
{
	fprintf(stderr, "0 setgray\n");
	print_triangle(t);
}

void print_triangle_gray(triangle* t)
{
	fprintf(stderr, "0.4 setgray\n");
	print_triangle(t);
}

void print_circle(float radius, vector centre)
{
	fprintf(stderr, "newpath\n");
	fprintf(stderr, "0 setgray\n");
	fprintf(stderr, "%f %f %f 0 360 arc\n", 
		centre[0]*SIZE+OFFX, centre[1]*SIZE+OFFY, radius*SIZE);
	fprintf(stderr, "stroke\n");
	
} 


int
circle_segment(vector c, float radius, vector a, vector b)
{
	float t, d, l, r1, r2, rr;
	float x1, y1, x2, y2, xc, yc, xh, yh;
	
	/* convert everything into double */
	x1 = a[0];
	y1 = a[1];
	x2 = b[0];
	y2 = b[1];
	xc = c[0];
	yc = c[1];

	printf("%f\n", x1);
	printf("%f\n", y1);
	printf("%f\n", x2);
	printf("%f\n", y2);
	printf("%f\n", xc);
	printf("%f\n", yc);


	if (x1 == x2 && y1 == y2)
		return 0;

	/* endpoint distances^2 from center */
	r1 = (x1-xc)*(x1-xc) + (y1-yc)*(y1-yc); 
	r2 = (x2-xc)*(x2-xc) + (y2-yc)*(y2-yc);
	rr = radius*radius;	

	printf("radius=%f\n", radius);
	printf("r1=%f\n", r1);
	printf("r2=%f\n", r2);
	printf("rr=%f\n", rr);

	if ( r1<rr || r2<rr )
		return 1;

	/* Now, both points are out */

	/* segment size^2 */
	l = (x2-x1)*(x2-x1) + (y2-y1)*(y2-y1);

	/* scalar product center-segment */
	d = (x2-x1)*(xc-x1) + (y2-y1)*(yc-y1);


	if (d>0 && d<l) {
		/* center-segment vector*/
		xh = (x2-x1)*d/l + x1-xc;
		yh = (y2-y1)*d/l + y1-yc;
		
		if ( xh*xh + yh*yh < rr )
			return 1;
	}
		
	return 0;
}


int circle_triangle(vector c, float radius, triangle* t)
{

	float D, Ds, Dt;
	float x1, y1, x2, y2, x3, y3, cx, cy;

	x1 = t->v1[0];
	y1 = t->v1[1];
	x2 = t->v2[0];
	y2 = t->v2[1];
	x3 = t->v3[0];
	y3 = t->v3[1];
	cx = c[0];
	cy = c[1];

	/* Is the center inside the triangle? */
	D  = (x2-x1)*(y3-y1) - (x3-x1)*(y2-y1);	
	Ds = (cx-x1)*(y3-y1) - (cy-y1)*(x3-x1); 
	Dt = (x2-x1)*(cy-y1) - (cx-x1)*(y2-y1);

	printf("D=%f\n", D);
	printf("Ds=%f\n", Ds);
	printf("Dt=%f\n", Dt);
	printf("s=%f\n", Ds/D);
	printf("t=%f\n", Dt/D);

	if ( D < 0) {
		D = -D;
		Ds = -Ds;
		Dt = -Dt;
	}

	if (Dt>=0 && Ds>=0 && Ds+Dt<=D) {
		printf("centre is in\n");
		return 1;
	}

	printf("centre is out\n");

	// print_point(t->v2);

	/* Does the circle intersect on side of the triangle */
	if (circle_segment(c, radius, t->v1, t->v2)) {
		printf("segment 1 intersect\n");
		return 1;
	}

	if (circle_segment(c, radius, t->v1, t->v3)) {
		printf("segment 2 intersect\n");
		return 1;
	}
	
	if (circle_segment(c, radius, t->v2, t->v3)) {
		printf("segment 3 intersect\n");
		return 1;
	}
		
	printf("no segments intersect\n");

	return 0;	
}


void random_triangle(triangle* t)
{
	t->v1[0] = drand48();
	t->v1[1] = drand48();
	t->v2[0] = drand48();
	t->v2[1] = drand48();
	t->v3[0] = drand48();
	t->v3[1] = drand48();
}

int
main(int argc, char* argv[])
{
	int i=0;
	float radius;
	vector centre;
	struct timeval tv;

	gettimeofday(&tv, NULL);
	srand48(atoi(argv[1]));
	// srand48(28);

	triangle tt;
	
	/*random circle*/
	radius = 0.2;
	centre[0] = 0.5;
	centre[1] = 0.5;
	print_circle(radius, centre);

	for(i=0; i<400; i++) {
		printf("-----------------%d\n", i);
		random_triangle(&tt);
		if (circle_triangle(centre, radius, &tt)) {
			//print_triangle_black(&tt);
			printf("inside\n");
		} else {
			print_triangle_gray(&tt);
			printf("outside\n");
		}
	}


	printf("Circle\n");

}


