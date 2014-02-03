#include <stdio.h>
#include <stdlib.h>
#include "opt.h"

int size;

int parse(FILE* fp)
{
int rc, i;
int count;
char c;

	count = 0;
	/* count the tags */
	while( (rc=fgetc(fp)) > 0 ){
		c = (char)rc;
		size++;
		if (c=='%') { 
			rc = fgetc(fp);
			if (rc>=0)
				size++;
			if ( rc>48 && rc<=56 )
				// found a tag
				count++;
		}
	}
	rewind(fp);

	if (count==0)
		return;

	/* build the template */
	tmpl = malloc(count*sizeof(int));
	
	i = 0;
	while( (rc=fgetc(fp)) > 0 ){
		c = (char)rc;
		if (c=='%') { 
			rc = fgetc(fp);
			if ( rc>48 && rc<=56 ) {
				// found a tag
				tmpl[i] = rc-48;
				i++;
			}
		}
	}
	rewind(fp);

	return count;
}

char* makeinst(char*s, FILE*fp) 
{
int i = 0, rc, offs=0;
char c;
char* out;
	
	/* create string */
	out = malloc(size+genesize*3);

	/* count the tags */
	while( (rc=fgetc(fp)) > 0 ){
		c = (char)rc;
		if (c=='%') { 
			rc = fgetc(fp);
			c = (char)rc;
			if ( rc>48 && rc<=56 ) {
				// found a tag
				offs += sprintf(out+offs, "%u", s[i]&0xFF);
				i++;
			} else {
				offs += sprintf(out+offs, "%%");
				if (c != '%')
					offs += sprintf(out+offs, "%c", (char)rc);
			}
		} else {
			offs += sprintf(out+offs, "%c", c);
		}
	}
	printf("%s\n", out);

	return out;	
}
