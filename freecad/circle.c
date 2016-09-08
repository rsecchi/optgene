#include <stdio.h>

#define SIZE 100
#define OFFX 200
#define OFFY 300

typedef float vector[2];
typedef struct 
{
	vector a, b, c;
} triangle;


int print_point(vector pp)
{
	fprintf(stderr, "newpath\n");
	fprintf(stderr, "%f %f 3 0 360 arc\n", pp[0]*SIZE+OFFX, pp[1]*SIZE+OFFY);
	
}


int
print(vector c, float r, vector a, vector b)
{
	fprintf(stderr, "newpath\n");
	fprintf(stderr, "%f %f %f 0 360 arc\n", c[0]*SIZE+OFFX, c[1]*SIZE+OFFY, r*SIZE);
	fprintf(stderr, "%f %f moveto\n", a[0]*SIZE+OFFX, a[1]*SIZE+OFFY);
	fprintf(stderr, "%f %f lineto\n", b[0]*SIZE+OFFX, b[1]*SIZE+OFFY);
	fprintf(stderr, "stroke\n");

	fprintf(stderr, "newpath\n");
	fprintf(stderr, "%f %f moveto\n", (double)(0*SIZE+OFFX), (double)(1*SIZE+OFFY));
	fprintf(stderr, "%f %f lineto\n", (double)(0*SIZE+OFFX), (double)(0*SIZE+OFFY));
	fprintf(stderr, "%f %f moveto\n", (double)(1*SIZE+OFFX), (double)(0*SIZE+OFFY));
	fprintf(stderr, "%f %f lineto\n", (double)(0*SIZE+OFFX), (double)(0*SIZE+OFFY));
	fprintf(stderr, "stroke\n");

}

void
print_triangle()
{
}


int
circle_segment(vector c, float radius, vector a, vector b)
{
	float t, d, l, r1, r2, rr;
	double x1, y1, x2, y2, xc, yc;
	double xh, yh;

	if (a[0] == b[0] && a[1] == b[1])
		return 0;

	/* convert everything into double */
	x1 = a[0];
	y1 = a[1];
	x2 = b[0];
	y2 = b[1];
	xc = c[0];
	yc = c[1];


	/* endpoint distances^2 from center */
	r1 = (x1-xc)*(x1-xc) + (y1-yc)*(y1-yc); 
	r2 = (x2-xc)*(x2-xc) + (y2-yc)*(y2-yc);
	rr = radius*radius;	

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


int circle_triangle(vector c, float radius, triangle t)
{

	/* Is the center inside the triangle? */
	

	/* Does the circle intersect on side of the triangle */
	if (circle_segment(c, radius, t.a, t.b))
		return 1;

	if (circle_segment(c, radius, t.a, t.c))
		return 1;
	
	if (circle_segment(c, radius, t.b, t.c))
		return 1;

	return 0;	
}



int
main()
{
	float r;
	vector c, a, b;

	c[0] = 0;
	c[1] = 1;
	r = 2;

	a[0] = -1;
	a[1] = 1;
	b[0] = 0;
	b[1] = 0;


	circle_segment(c, r, a, b);
	print(c, r, a, b);

	printf("Circle\n");

}


