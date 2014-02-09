#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

int eval(char* s)
{
	char buf[30];
	int com[2];
	int size;

	pipe(com);
	if (fork()) {
		close(com[0]);
		write(com[1], s, 5);
		exit(1);
	}

	close(com[1]);
	while(size = read(com[0], buf, 30)){
		buf[size] = '\0';
		printf("%s",buf);
	}

}

int main()
{
	char buffer[30];

	eval("Raffaello");

}

