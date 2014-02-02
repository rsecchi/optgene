#include <stdio.h>
#include <stdlib.h>

int count;
int* tmpl;

void parse(FILE* fp)
{
int rc, offs, i;
char c;

	offs = 0;

	/* count the tags */
	while( (rc=fgetc(fp)) > 0 ){
		c = (char)rc;
		if (c=='%') { 
			rc = fgetc(fp);
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

}



