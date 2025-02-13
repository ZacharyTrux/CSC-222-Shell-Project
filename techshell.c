#include <stdio.h>

char* CommandPrompt(); // recieve input
struct ShellCommand ParseCommandLine(char* input); // figure out what the command is 
void ExecuteCommand(struct ShellCommand command); // execute the command 

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