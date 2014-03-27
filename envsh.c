// envsh shell for Brian Scheper and Andrew Johnston

#include <stdio.h> 
#include <stdlib.h>

# define MAXLINE 256

int main() {

	char command[MAXLINE];

	while(1) {
		printf("envsh> ");
		fgets(command, MAXLINE, stdin);
		if (feof(stdin)) 
			exit(0);

	}
	return 0;
}
