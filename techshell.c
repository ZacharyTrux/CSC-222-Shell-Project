#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <limits.h>

#define DEBUG

char* CommandPrompt(); // recieve input
//struct ShellCommand ParseCommandLine(char* input); // figure out what the command is 
//void ExecuteCommand(struct ShellCommand command); // execute the command 

int main(){
    char* input;
    //struct ShellCommand command;

    for(;;){
        input = CommandPrompt();
        //command = ParseCommandLine(input);
        //ExecuteCommand(command);
    }

    exit(0);
}

char* CommandPrompt(){
    char buffer[PATH_MAX + 1];
    char *cwd = getcwd(buffer, PATH_MAX + 1); // gets the working directory
    printf("%s$ ", cwd);



    char *input;
    input = (char*)malloc(50 * sizeof(char));
    scanf("%s", input);
    //printf("\n");
    
    return input;
}
