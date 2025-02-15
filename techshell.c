#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <fcntl.h>

// defines constants for maximum input size and argument count
#define MAX_INPUT 1024
#define MAX_ARGS 64

// structure to hold parsed command information
struct ShellCommand {
    char *args[MAX_ARGS];  // array to store command arguments
    char *input_file;      // input file for redirection
    char *output_file;     // output file for redirection
};

// function to display the command prompt and read user input
char* CommandPrompt(){
    static char cwd[1024];

    // gets the current working directory and displays it
    if (getcwd(cwd, sizeof(cwd)) == NULL){
        perror("getcwd error");
        exit(EXIT_FAILURE);
    }
    printf("%s$ ", cwd);
    fflush(stdout);

    // allocates memory for user input and read it from stdin
    char *input = malloc(MAX_INPUT);
    if (!fgets(input, MAX_INPUT, stdin)){
        free(input);
        exit(0);
    }

    // removes the trailing newline character from input string
    input[strcspn(input, "\n")] = 0;
    return input;
}

// fucntion to parse the command line input into a ShellCommand structure
struct ShellCommand ParseCommandLine(char *input){
    struct ShellCommand command;
    command.input_file = NULL;
    command.output_file = NULL;

    int argc = 0;
    char *token = strtok(input, " "); // tokenizes input by spaces

    while (token){
        // checks for input redirection and sets the input file
        if (strcmp(token, "<") == 0){
            token = strtok(NULL, " ");
            command.input_file = token;
        } 
        // checks for output redirection and sets the output file
        else if (strcmp(token, ">") == 0){
            token = strtok(NULL, " ");
            command.output_file = token;
        } 
        // otherwise, treat the token as a command argument
        else {
            command.args[argc++] = token;
        }
        token = strtok(NULL, " ");
    }
    // ensures the last argument is NULL (required by execvp)
    command.args[argc] = NULL;
    return command;
}

// function to execute the parsed command
void ExecuteCommand(struct ShellCommand command){
    if (command.args[0] == NULL) return;

    // handle the built-in "exit" command to terminate the shell
    if (strcmp(command.args[0], "exit") == 0){
        exit(0);
    }

    // handles the built-in "cd" command to change directories
    if (strcmp(command.args[0], "cd") == 0){
        if (command.args[1]){
            // changes to the specified directory and handles errors
            if (chdir(command.args[1]) != 0){
                printf("Error %d (%s)\n", errno, strerror(errno));
            }
        }
        return; // returns if the cd command was executed
    }

    // creates a child process to execute external commands
    pid_t pid = fork();
    if (pid < 0){
        perror("fork failed"); // if fork fails, print error and exit
        exit(EXIT_FAILURE);
    }
    if (pid == 0){ // child process
        // handles input redirection if specified
        if (command.input_file){
            int in_fd = open(command.input_file, O_RDONLY); // opens input file for reading
            if (in_fd < 0){
                perror("Input file error");
                exit(EXIT_FAILURE);
            }
            dup2(in_fd, STDIN_FILENO); // redirects standard input to the file
            close(in_fd); // closes the file descriptor after redirection
        }

        // handles output redirection if specified
        if (command.output_file){
            int out_fd = open(command.output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644); // open output file for writing
            if (out_fd < 0){
                perror("Output file error");
                exit(EXIT_FAILURE);
            }
            dup2(out_fd, STDOUT_FILENO); // redirects standard output to the file
            close(out_fd); // closes the file descriptor after redirection
        }

        // executes the command using execvp
        execvp(command.args[0], command.args);
        // if execvp fails, print the error and exit
        printf("Error %d (%s)\n", errno, strerror(errno));
        exit(EXIT_FAILURE); // exits the child process if execvp fails
    }
    wait(NULL); // parent process waits for child to finish
}

// main fucntion - starts the shell loop
int main(){
    while (1){ // infinite loop to keep the shell running
        char *input = CommandPrompt(); // displays prompt and get input
        struct ShellCommand command = ParseCommandLine(input); // parses the input into a command structure
        ExecuteCommand(command); // execute the parsed command
        free(input); // free the allocated memory for input
    }
    return 0;
}