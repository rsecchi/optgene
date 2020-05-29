#include "opt.h"

int running;
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

double eval(char *s)
{
	int testfd;

	int status = 0;

	char buf[256];
	char tempfname[] = "tempXXXXXX";
	int com[2];
	int rd;


	if (pipe(com)) {
		fprintf(stderr, "cannot open pipe for reading\n");
		exit(1);
	}

	/* creates a temporary file */
	testfd = mkstemp(tempfname);
	if (!testfd) {
		perror(NULL);
		exit(1);
	}

	// create a file to be evaluated
	makeinst(s, script, testfd);
	chmod(tempfname, S_IRUSR | S_IXUSR);
	fsync(testfd);
	close(testfd);

	if (!fork()) {
		close(com[0]);

		dup2(com[1], STDOUT_FILENO);
		if (system(tempfname))
			fprintf(stderr, "could not execute %s\n",
				tempfname);
		exit(0);
	}

	close(com[1]);
	wait(&status);

	while ((rd = read(com[0], buf, 255)))
		buf[rd] = '\0';

	close(com[0]);
	if (remove(tempfname))
		fprintf(stderr, "could not remove %s\n", 
			tempfname);

	if (!running)
		exit(0);

	return atof(buf);
}

