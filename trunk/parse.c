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

void makeinst(char *s, FILE * fp, FILE * out)
{

	int rc, i = 0;
	char c;
	rewind(fp);

	/* count the tags */
	while ((rc = fgetc(fp)) > 0) {
		c = (char) rc;
		if (c == '%') {
			rc = fgetc(fp);
			c = (char) rc;
			if (rc > 48 && rc <= 56) {
				// found a tag
				fprintf(out, "%u", s[i] & 0xFF);
				// printf("%u", s[i] & 0xFF);
				i++;
			} else {
				fprintf(out, "%%");
				// printf("%%");
				if (c != '%') {
					fprintf(out, "%c", (char) rc);
					// printf("%c", (char) rc);
				}
			}
		} else {
			fprintf(out, "%c", c);
			// printf("%c", c);
		}
	}

}