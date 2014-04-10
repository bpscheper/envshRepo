// envsh shell for Brian Scheper and Andrew Johnston

#include <stdio.h> 
#include <stdlib.h>
#include <string.h>

# define MAXLINE 256

typedef struct {
	char token[MAXLINE];
	char* type;
} Element;

typedef struct{
	char name[MAXLINE];
	char value[MAXLINE];
	int active;
} Variable;

//Globals
char prompt[MAXLINE] = "envsh > ";
Variable envVars[MAXLINE];

//Prototypes
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
			printf("%s", prompt);
		}
		parser();
	}
	return 0;
}

void scanner(char command[], Element* each_element) {

	int current = 0; // current place in element array 
	int i; // index used for command line 
	for (i = 0; i < MAXLINE; ++i) {
		//First line is a special case, check for built-in commands
		if (i ==0){
			//Check for a comment
			if ((command[i] == '%')) {
				int index = 0;
				char comment[MAXLINE];
				while (command[i] != '\n') {
					comment[index] = command[i];
					++i;
					++index;
				}
				each_element[current].type = "metachar";
				strncpy(each_element[current].token, comment, sizeof(comment)/sizeof(*comment));
				return;
			}
			else {
				//Check for built in commands
				char firstcmd[6];
				strncpy(firstcmd, command, 6);
				if (strcmp(firstcmd, "prompt") == 0){
					each_element[current].type = "prompt";
					strcpy(each_element[current].token, "prompt");	
					++current;
				} else if (strcmp(firstcmd, "setdir") == 0) {
					each_element[current].type = "setdir";
					strcpy(each_element[current].token, "setdir");	
					++current;
					continue;
				} else if (strcmp(firstcmd, "setenv") == 0) {
					each_element[current].type = "setenv";
					strcpy(each_element[current].token, "setenv");	
					++current;
					continue;
				}

				char check_unset[8];
				strncpy(check_unset, command, 8);
				if (strcmp(check_unset, "unsetenv") == 0) {
					each_element[current].type = "unsetenv";
					strcpy(each_element[current].token, "unsetenv");	
					++current;
					continue;
				}

				char list_env[7];
				strncpy(list_env, command, 7);
				if (strcmp(list_env, "listenv") == 0) {
                    each_element[current].type = "listenv";
					strcpy(each_element[current].token, "listenv");	
                    ++current;
					continue;
				}

				char bye[3];
                strncpy(bye, command, 3);
                if (strcmp(bye, "bye") == 0) {
                	each_element[current].type = "bye";
					strcpy(each_element[current].token, "bye");	
                    ++current;
                    continue;
                }
			}
		} else if (command[i] == '<') {
			++current;
           	each_element[current].type = "metachar";
			strcpy(each_element[current].token, "<");
			continue;
		} else if (command[i] == '>') {
			++current;
			each_element[current].type = "metachar";
			strcpy(each_element[current].token, ">");	
			continue;
		} else if (command[i] == ' ') {
			if (strcmp(each_element[current-1].type, "setenv") == 0){
				++i;
				char str[MAXLINE];
				int len = 0;
				while(command[i] != ' '){
					str[len] = command[i];
					str[len + 1] = '\0';
	                ++len;
	                ++i;
				}

				each_element[current].type = "stringliteral";
	            strcpy(each_element[current].token, str);

    	        ++current;
			}
		} else if (command[i] == '"') { 
			++i;
			char str[MAXLINE];
			int len = 0;
			while (command[i] != '"') {
				str[len] = command[i];
				str[len + 1] = '\0';
				++len;
				++i;
			}

			each_element[current].type = "string";
			strcpy(each_element[current].token, str);

			++current;
		} else {

		}
	}
}

void parser() {
	char command[MAXLINE]; // command line input
	Element each_element[MAXLINE]; // array of tokens/type of each 
	
	//Clear our array
	memset(&each_element[0], 0, sizeof(each_element));

	//int cur_element = 0; // current index of token from command line

	fgets(command, MAXLINE, stdin);
	if (feof(stdin))
		exit(0);

	//Scan all of the elements
	scanner(command, each_element);

	//Parse each of the elements
	int i = 0;
	for(i=0; i < MAXLINE; i++){
		//Built-In Commands
		//Set the shell prompt to next token
		if(each_element[i].type == "prompt"){
			strcpy(prompt, each_element[i+1].token);
		}
		//Set the environment variable
		else if(each_element[i].type == "setenv"){
			int i =0;
			while (i < MAXLINE){
				if (envVars[i].active != 1){
					strcpy(envVars[i].name, each_element[i+1].token);
					strcpy(envVars[i].value, each_element[i+2].token);
					envVars[i].active = 1;
					break;
				}
				i++;
			}
		}
		//Unset the environment variable
		else if(each_element[i].type == "unsetenv"){
		}
		//Print the current environment variables and values
		else if(each_element[i].type == "listenv"){
			int i = 0;
			while(i < MAXLINE){
				if (envVars[i].active == 1){
					printf("%s=\"%s\"\n", envVars[i].name, envVars[i].value);
				}
				++i;
			}
		}
		//Set current directory
		else if(each_element[i].type == "setdir"){
		}
		//Exit
		else if(each_element[i].type == "bye"){
			exit(0);
		}
		//Non built in function, fork here
		else{
			int pid = fork();
			if (pid == 0){ //Child
				execl("/bin/date","",NULL);
			}
			wait(pid);
			break;
		}
	}
}
