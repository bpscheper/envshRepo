// envsh shell for Brian Scheper and Andrew Johnston

#include <stdio.h> 
#include <stdlib.h>

# define MAXLINE 256

int scanner(char* command, char* token, char* type, int pos);
void parser();

int main() {

	while(1) {
		printf("envsh> ");
		parser();
	}
	return 0;
}

int scanner(char* command, char* token, char* type, int pos) {

	for (; command[pos] != ' '; pos++) {
		char character = command[pos];
		if ((character == '%') || (character == '>') || (character == '<')) {
			type = "metachar";
		}
	}

	return pos;
}

void parser() {

	char command[MAXLINE];
	char token[MAXLINE];
	char type[MAXLINE];

	fgets(command, MAXLINE, stdin);
	if (feof(stdin))
		exit(0);
	int position = scanner(command, token, type, 0);
	while(token != '\0') {
		position = scanner(command, token, type, position);
	}
}
