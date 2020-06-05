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

double eval(struct tctx* p)
{
	int testfd;
	int pid;
	int wstatus;

	char* s = p->seg->string;
	char buf[256], *bp = buf;
	char tempfname[] = "./tempXXXXXX";
	char* prog[2];
	int com[2];
	int rd;
	int retries;
	double result;
	int timeout;

	pthread_mutex_lock(&eval_mutex);

	if (pipe2(com, O_NONBLOCK)) {
		fprintf(stderr, "cannot open pipe for reading\n");
		exit(1);
	}

	/* creates a temporary file */
	testfd = mkstemp(tempfname);
	if (!testfd) {
		perror(NULL);
		exit(1);
	}

	/* creating a file to be evaluated  */
	makeinst(s, script, testfd);

	chmod(tempfname, S_IRUSR | S_IXUSR);
	fsync(testfd);
	close(testfd);

	pthread_mutex_unlock(&eval_mutex);

	/* starting the evaluating process */
	pid = fork();
	if (pid == 0) {
		close(com[0]);
		// fprintf(stderr, "start\n");
		dup2(com[1], STDOUT_FILENO);

		prog[0] = tempfname;
		prog[1] = NULL;

		retries = 0;
		do {
			execve(prog[0], prog, NULL);
			rd = errno;
			sleep(1);
			retries++;
		} while (rd==ETXTBSY && retries<4);
		perror(tempfname);
		exit(1);
	}

	pthread_mutex_lock(&eval_mutex);
	close(com[1]);
	pthread_mutex_unlock(&eval_mutex);

	timeout = 0;
	/* reading from pipe */
	while ((rd = read(com[0], bp, 255)) != 0) {
		if (rd<0 && errno==EAGAIN) {
			timeout++;
			if (timeout == TIMEOUT){
				fprintf(stderr, "TIMEOUT\n");
				kill(pid, SIGTERM);
				wait(&wstatus);
				break;
			}
			sleep(1);
			continue;
		}

		if (rd < 0) {
			perror("error reading from pipe: ");
			exit(1);
		}
	
		bp += rd;
		if (bp - buf > 255) {
			buf[255] = '\0';
			fprintf(stderr, "message too long: %s\n", buf);
		}
	}
	*bp = '\0';

	pthread_mutex_lock(&eval_mutex);
	close(com[0]);

	if (remove(tempfname)) {
		perror("could not remove temp file"); 
		exit(1);
	}

	if (!running) {
		pthread_mutex_unlock(&eval_mutex);
		pthread_exit(0);
	}

	pthread_mutex_unlock(&eval_mutex);

	result = strtod(buf, &bp);

	p->seg->flags = (bp==buf) ? INVALID : RATED; 


	return result;
}

