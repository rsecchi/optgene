#include <stdio.h>
#include <stdlib.h>
#include "opt.h"

int size;

int parse(FILE* fp)
{
int rc, i;
int count;
char c;

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

	return count;
}


void maketest(char*s, FILE*fp) 
{
	
}
