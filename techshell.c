#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <limits.h>

#define DEBUG 1
#define MAX_ARGS 10
#define MAX_COMMAND_LENGTH 100

char* CommandPrompt(); // recieve input
struct ShellCommand ParseCommandLine(char* input); // figure out what the command is 
void ExecuteCommand(struct ShellCommand command); // execute the command 

int main(){
    char* input;
    struct ShellCommand command;

    for(;;){
        input = CommandPrompt();
        command = ParseCommandLine(input);
        if(DEBUG){printf("input: %s, command: %s\n", input, command);}
        ExecuteCommand(command);
    }

    exit(0);
}

struct ShellCommand {
    char *command[MAX_COMMAND_LENGTH];
    char *args[MAX_ARGS][MAX_COMMAND_LENGTH];
    int num_args;
}
struct ShellCommand ParseCommandLine(char *input){
    struct ShellCommand commandLine;
    commandLine.num_args = 0;
    commandLine.command = input[0];

    int size = sizeof(input) / sizeof(char*);

    for(int i = 1; i < size; i++){
        commandLine.args[i][] = input[i];
        for (int j = i; j <= i; j++){
            commandLine.args[i][j] = sizeof(input[i]) / sizeof(char*);  
        }
        commandLine.num_args++;
    }

    return commandLine;
}

void ExecuteCommand(struct ShellCommand commandLine){
    pid_t p = fork();
    if(p < 0){
        perror("Fork Failed");
        exit(1);
    }
    execvp(commandLine.command, commandLine.args);
}

char* CommandPrompt(){
    char buffer[PATH_MAX + 1];
    char *cwd = getcwd(buffer, PATH_MAX + 1); // gets the working directory
    printf("%s$ ", cwd); // print out the current directory similar to that of the commmand line

    char *input; 
    input = (char*)malloc(50 * sizeof(char)); // allocate memory to store the input 
    scanf("%s", input); 
    
    return input;
}
