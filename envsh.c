// envsh shell for Brian Scheper and Andrew Johnston

#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

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
	
	//Set up quiet mode
	int quiet = 0;
	if (argc >= 2 && strcmp(argv[1],"quiet") == 0){
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
	int end_word = 0;
	int mid_word = 0;
	char word[MAXLINE];
	int index_word = 0;
	for (i = 0; i < MAXLINE; ++i) {
		
		if (end_word == 1) {
			mid_word = 0;
			index_word = 0;
			end_word = 0;
			memset(word, 0, MAXLINE);
		}
		//First line is a special case, check for built-in commands
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
		} else if (command[i] == '<') {
			if ((current > 0) || (mid_word == 1)) {
				each_element[current].type = "word";
				strcpy(each_element[current].token, word);
				++current;
			}
           		each_element[current].type = "metachar";
			strcpy(each_element[current].token, "<");
			++current;
		} else if (command[i] == '>') {
			if ((current > 0) || (mid_word == 1)) {
				each_element[current].type = "word";
				strcpy(each_element[current].token, word);
				++current;
			}
			each_element[current].type = "metachar";
			strcpy(each_element[current].token, ">");
			++current;
		} else if ((command[i] == ' ') && (mid_word == 1)){
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
			end_word = 1;
			each_element[current].type = "word";
			strcpy(each_element[current].token, word);
			++current;
			mid_word = 0;
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
		} else if (command[i] == '\n') {
			each_element[current].type = "word";
			strcpy(each_element[current].token, word);
			++current;
			each_element[current].type = "end-of-line";
			strcpy(each_element[current].token, "EOL");
			++current;
		} else {
			mid_word = 1;
			word[index_word] = command[i];
			word[index_word+1] = '\0';
			++index_word;
		}
	}
	
	if (strcmp(each_element[0].token, "prompt") == 0) {
		each_element[0].type = "prompt";
	} else if (strcmp(each_element[0].token, "setenv") == 0) {
		each_element[0].type = "setenv";
	} else if (strcmp(each_element[0].token, "setdir") == 0) {
		each_element[0].type = "setdir";
	} else if (strcmp(each_element[0].token, "listenv") == 0) {
		each_element[0].type = "listenv";
	} else if (strcmp(each_element[0].token, "unsetenv") == 0) {
		each_element[0].type = "unsetenv";
	} else if (strcmp(each_element[0].token, "bye") == 0) {
		each_element[0].type = "bye";
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

	int j = 0;
	while (each_element[j].type != "end-of-line") {
//		printf("%s\n", each_element[j].token);
		j++;
	}

	//Built-In Commands
	//Set the shell prompt to next token
	if(each_element[0].type == "prompt"){
		strcpy(prompt, each_element[1].token);
	}
	//Set the environment variable
	else if(each_element[0].type == "setenv"){
		int i = 0;
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
	else if(each_element[0].type == "unsetenv"){
		int i = 0;
		while(i < MAXLINE){
			if (strcmp(envVars[i].name, each_element[i+1].token) == 0)
				envVars[i].active = 0;
			++i;
		}
	}
	//Print the current environment variables and values
	else if(each_element[0].type == "listenv"){
		int i = 0;
		while(i < MAXLINE){
			if (envVars[i].active == 1){
				printf("%s=\"%s\"\n", envVars[i].name, envVars[i].value);
			}
			++i;
		}
	}
	//Set current directory
	else if(each_element[0].type == "setdir"){
		int i = chdir(each_element[1].token);
		if (i != 0){
			printf("the directory could not be changed.\n");
		}
	}
	//Exit
	else if(each_element[0].type == "bye"){
		exit(0);
	}
	else if (command[0] == '%') {
		return;
	}
	//Non built in function, fork here
	else{
		if (each_element[0].type != "word") {
			printf("invalid command. %s not a valid command\n", each_element[0].type);
			return;
		} else {
			int i;
			int if_output = 0;
			int if_input = 0;
			char input[MAXLINE];
			char output[MAXLINE];
			int num_args = -1;
			int end_args = 0; // end of arguments for command
			for (i = 0; i < MAXLINE; ++i) {
				if (strcmp(each_element[i].token, ">") == 0) {
					++i;
					if_output = 1;
					strcpy(output, each_element[i].token);
					end_args = 1;
				} else if (strcmp(each_element[i].token, "<") == 0) {
					++i;
					if_input = 1;
					strcpy(input, each_element[i].token);
					end_args = 1;
				} else if (strcmp(each_element[i].token, "EOL") == 0) {
					end_args = 1;
				} else {
					if (end_args == 0) {
						num_args++;
					}
				}
			}
			pid_t pid = fork();
			if (pid == 0){ //Child
				if (if_output == 1) {
					int permission = 0777;
					int fd = open(output, O_CREAT|O_WRONLY, permission);
					printf("%d\n", fd);
					if (fd == -1) 
						perror("Output Error: ");
					int fd_dup = dup2(fd, 1);
					close(fd);
				}
				if (if_input == 1) {
					int fd = open(input, O_RDONLY);
					if (fd == -1)
						perror("Input Error: ");
					int fd_dup = dup2(fd, 0);
					close(fd);
				}
				execve(each_element[0].token, args, environ, NULL);
			}
			wait(pid);
		}
	}
	
}
