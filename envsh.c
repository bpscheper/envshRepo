// envsh shell for Brian Scheper and Andrew Johnston

#include <stdio.h> 
#include <stdlib.h>

# define MAXLINE 256

int main() {

	while(1) {
		printf("envsh> ");
		parser();
	}
	return 0;
}

void scanner(char* command, char* token, char* type) {



}

void parser() {

	char command[MAXLINE];
	char token[];
	char type[];

	fgets(command, MAXLINE, stdin);
	if (feof(stdin))
		exit(0);
	scanner(command, token, type);
	while() {

	}
}
