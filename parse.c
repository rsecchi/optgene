#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "opt.h"

int size;

int parse(FILE * fp)
{
	int rc, i;
	int count;
	char c;

	count = 0;
	/* count the tags */
	while ((rc = fgetc(fp)) > 0) {
		c = (char) rc;
		size++;
		if (c == '%') {
			rc = fgetc(fp);
			if (rc >= 0)
				size++;
			if (rc > 48 && rc <= 56)
				// found a tag
				count++;
		}
	}
	rewind(fp);

	if (count == 0)
		return 0;

	/* build the template */
	tmpl = malloc(count * sizeof(int));

	i = 0;
	while ((rc = fgetc(fp)) > 0) {
		c = (char) rc;
		if (c == '%') {
			rc = fgetc(fp);
			if (rc > 48 && rc <= 56) {
				// found a tag
				tmpl[i] = rc - 48;
				i++;
			}
		}
	}
	rewind(fp);

	return count;
}

void makeinst(char *s, FILE * fp, int out)
{

	int rc, i = 0, j, n;
	char c, buf, tmp[8];

	rewind(fp);

	/* count the tags */
	while ((rc = fgetc(fp)) > 0) {
		c = (char) rc;
		if (c == '%') {
			rc = fgetc(fp);
			c = (char) rc;
			if (rc > 48 && rc <= 56) {
				// found a tag
				n = sprintf(tmp, "%u", s[i] & 0xFF);
				for(j=0; j<n; j++)
					write(out, &tmp[j], 1);
				i++;
			} else {
				buf = '%';
				write(out, &buf, 1);
				if (c != '%') {
					write(out, &c, 1);
				}
			}
		} else {
			write(out, &c, 1);
		}
	}

}
