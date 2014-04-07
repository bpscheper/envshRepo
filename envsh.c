// envsh shell for Brian Scheper and Andrew Johnston

#include <stdio.h> 
#include <stdlib.h>

# define MAXLINE 256

typedef struct {
	char* token;
	char* type;
} Element;

int changed_prompt = 0;
char prompt[MAXLINE];

void scanner(char* command, Element* each_element);
void parser();

int main(int argc, char *argv[]) {

//quiet mode is not working at the moment. I can't get it to work
	int quiet = 0;
	if (argv[1] == "quiet") {
		quiet = 1;
	}
	while(1) {
		if (quiet == 0) {
			if (changed_prompt == 0) {
				printf("envsh> ");
			} else {
			}
		}
		parser();
	}
	return 0;
}

void scanner(char* command, Element* each_element) {

	int current = 0; // current place in element array 
	int i; // index used for command line 
	for (i = 0; i < MAXLINE; ++i) {
		if ((command[i] == '%') && (i == 0)) {
			int index = 0;
			char comment[MAXLINE];
			while (command[i] != '\n') {
				comment[index] = command[i];
				++i;
				++index;
			}
			each_element[current].type = "metachar";
			each_element[current].token = comment;
		} else if (command[i] == '<') {

                        ++current;
                        each_element[current].type = "metachar";
                        each_element[current].token = "<";
		} else if (command[i] == '>') {

			++current;
			each_element[current].type = "metachar";
			each_element[current].token = ">";
		} else if (command[i] == ' ') {

		} else if (command[i] == '"') { 
			while (command[i] != '"') {
				
				++i;
			}
			each_element[current].type = "string";
		} else {

		}
	}
}

void parser() {

	char command[MAXLINE]; // command line input
	Element each_element[MAXLINE]; // array of tokens/type of each 
		// command line input
	int cur_element = 0; // current index of token from command line

	fgets(command, MAXLINE, stdin);
	if (feof(stdin))
		exit(0);
	scanner(command, each_element);
	while(each_element[cur_element].token != '\0') {
		scanner(command, each_element);
	}
}
