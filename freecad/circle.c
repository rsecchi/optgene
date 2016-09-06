#include <stdio.h>

#define SIZE 100
#define OFFX 200
#define OFFY 300

typedef float vector[2];


int
print(vector c, float r, vector a, vector b)
{
	fprintf(stderr, "newpath\n");
	fprintf(stderr, "%f %f %f 0 360 arc\n", c[0]*SIZE+OFFX, c[1]*SIZE+OFFY, r*SIZE);
	fprintf(stderr, "%f %f moveto\n", a[0]*SIZE+OFFX, a[1]*SIZE+OFFY);
	fprintf(stderr, "%f %f lineto\n", b[0]*SIZE+OFFX, b[1]*SIZE+OFFY);
	fprintf(stderr, "stroke\n");
}

int
circle(vector c, float radius, vector a, vector b)
{
	float t, q;

	if (a[0] == b[0] && a[1] == b[1])
		return 0;

	q = (b[0]-a[0])*(b[0]-a[0]) + (b[1]-a[1])*(b[1]-a[1]);
	t = (b[0]-a[0])*(a[0]-c[0]) + (b[1]-a[1])*(a[1]-c[1]);

	t  = t/q;

	printf("%f\n",t);
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


	circle(c, r, a, b);
	print(c, r, a, b);

	printf("Circle\n");

}


