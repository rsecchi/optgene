#include "opt.h"

int running;
int size;
pthread_mutex_t eval_mutex;


int parse(char* fp)
{

	int i, count;

	/* count tags */
	count = 0;
	for(i=0; i<scr_size-1; i++){
		if (fp[i]=='%' && fp[i+1]>48 && fp[i+1]<=56)
			count++;
		if (fp[i]=='%' && fp[i+1]=='%')
			i++;
	}

	if (count == 0)
		return 0;

	/* build the template */
	tmpl = malloc(count * sizeof(int));

	count = 0;
	for(i = 0; i<scr_size-1; i++) {
		if (fp[i]=='%' && fp[i+1]>48 && fp[i+1]<=56)
		{
			tmpl[count] = fp[i+1] - 48;
			count++;
		}
		if (fp[i]=='%' && fp[i+1]=='%')
			i++;
	}

	return count;
}

void makeinst(char *s, char* fp, int out)
{

	int i, j, k=0, n;
	char tmp[8];

	/* count the tags */
	for(i=0; i<scr_size-1; i++)
	{
		if (fp[i]=='%' && fp[i+1]>48 && fp[i+1]<=56)
		{
			n = sprintf(tmp, "%u", s[k] & 0xFF);
			for(j=0; j<n; j++)
				write(out, &tmp[j], 1);
			k++;
			i++;
			if (i==scr_size-1)
				return;
			continue;
		}
		write(out, &fp[i], 1);
		if (fp[i]=='%' && fp[i+1]=='%') {
			i++;
			if (i==scr_size-1)
				return;
			continue;
		}
	}
	
	write(out, &fp[scr_size-1], 1);

}

double eval(char *s)
{
	int testfd;
	int pid;

	int status = 0;

	char buf[256];
	char tempfname[] = "./tempXXXXXX";
	int com[2];
	int rd;

	pthread_mutex_lock(&eval_mutex);

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
	pthread_mutex_unlock(&eval_mutex);


	pid = fork();
	if (pid == 0) {
		close(com[0]);

		dup2(com[1], STDOUT_FILENO);

		if (system(tempfname))
			fprintf(stderr, "could not execute %s\n",
				tempfname);
		exit(0);
	}

	close(com[1]);
	waitpid(pid, &status, 0);

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

