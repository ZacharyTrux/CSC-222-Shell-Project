#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <limits.h>
#include <sys/wait.h>
#include <signal.h>
#include <ctype.h>

#define MAX_COMMAND_LENGTH 100

char* CommandPrompt(); // receive input
struct ShellCommand ParseCommandLine(char* input); // figure out what the command is 
void ExecuteCommand(struct ShellCommand command); // execute the command 
char* to_lowercase(char *str); 
struct ShellCommand{
    char *args[MAX_COMMAND_LENGTH];
    char *input_file;
    char *output_file;
};

int main(){
    char* input;
    struct ShellCommand command;

    for(;;){
        input = CommandPrompt();
        command = ParseCommandLine(input);
        ExecuteCommand(command);
    }

    exit(0);
}

struct ShellCommand ParseCommandLine(char *input){  
    struct ShellCommand command;
    command.input_file = NULL;
    command.output_file = NULL;

    int index = 0;
    // tokenizing is a way of creating sub-strings in which each token or substring is created based on spaces which indicate the token has ended 
    char *token = strtok(input, " "); // separate input by spaces using tokenization

    while(token){  
        if(strcmp(token, "<") == 0){ // strcmp compares two strings to see if they are similar, returning 0 if true 
            token = strtok(NULL, " "); // move to the next token AKA the input file if "<" is detected
            command.input_file = token;
        }
        else if(strcmp(token, ">") == 0){
            token = strtok(NULL, " ");
            command.output_file = token;
        }
        else{
            command.args[index++] = token;
        }
        token = strtok(NULL, " "); // iterates to the next token 
    }

    command.args[index] = NULL; // set the last index as NULL to allow for a later function (execvp) to work
    return command;
}

void ExecuteCommand(struct ShellCommand command){
    // consider user wanting to leave the command line
    if(strcmp(to_lowercase(command.args[0]), "exit") == 0){
        exit(0);
    }
    // handle changing directories 
    else if(strcmp(command.args[0], "cd") == 0){
        int num = chdir(command.args[1]);
        if(num < 0){ // if cd is wrong num will be -1
            perror("Error 13");
        }
        return; 
    }
    
    pid_t p = fork();
    if(p < 0){
        perror("Fork Failed");
        exit(1);
    }
    else if(p == 0){ // fork child process handles further 
        if(command.input_file){
            FILE* infile = fopen(command.input_file, "r");
            dup2(fileno(infile), 0);
            fclose(infile);
        }
        if(command.output_file){
            FILE* outfile = fopen(command.output_file, "w");
            dup2(fileno(outfile), 1);
            fclose(outfile);
        }
        int num = execvp(command.args[0], command.args);
        if(num < 0){
            perror("Error 2");
            exit(1);
        }
    
    }
    wait(NULL); // allows child to finish up
}

char* CommandPrompt(){
    char buffer[PATH_MAX + 1];
    char *cwd = getcwd(buffer, PATH_MAX + 1); // gets the working directory
    printf("%s$ ", cwd); // print out the current directory similar to that of the commmand line
    fflush(stdout); // clear the previous input


    char *input = (char*)malloc(50 * sizeof(char)); // allocate memory to store the input 
    if(!fgets(input, 50 * sizeof(char), stdin)){ // no input, free memory
        free(input);
        exit(0);
    }
    
    input[strcspn(input, "\n")] = 0; // gets rid of null char at the end
    return input;
}

char* to_lowercase(char *str){
    for (int i = 0; str[i]; i++) {
        str[i] = tolower(str[i]);
      }
    return str;
}