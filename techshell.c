//* Techshell - Aiden Plauche, Cullen Duval - CSC 222 001 - Winter 2022-2023 *//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <wait.h>
#define DEBUG 0

// Displays cwd and returns user input
char* CommandPrompt() {
    char* input = malloc(sizeof(char) * 256);
    char cwd[256];
    getcwd(cwd, sizeof(cwd));
    printf("%s$ ", cwd);
    
    fgets(input, 256, stdin);
    input[strlen(input) - 1] = '\0'; // replace newline with null

    if (strcmp(input, "exit") == 0) exit(0);

    return input;
}

// Wrapper for shell command
struct ShellCommand {
    char* binary; // the binary to execute (e.g., ls);
    char* arguments[256]; // the arguments to send to the binary;
    char* input_file; // file to redirect stdin to
    char* output_file; // file to redirect stdout to
    int length; // number of arguments
};

// Processes the user input as a Shell Command
struct ShellCommand ParseCommandLine(char* input) {
    // string parsing
    // check for i/o redirection first (< in, > out)
    struct ShellCommand command = {NULL, {""}, NULL, NULL, 0}; // initializes shellCommand structure

    // stores the arguments from input into the structure variable arguments
    command.arguments[command.length] = strtok(input, " "); // split input by spaces
    command.binary = command.arguments[command.length]; // sets binary to first argument
    char* arg = strtok(NULL, " ");
    while (arg != NULL) {
        if (strcmp(arg, "<") == 0) {
            command.input_file = strtok(NULL, " ");
            arg = strtok(NULL, " ");
            if (!arg) break;
            else if (strcmp(arg, ">") != 0) break;
        } 
        if (strcmp(arg, ">") == 0) {
            command.output_file = strtok(NULL, " ");
            break;
        }
        
        command.arguments[++command.length] = arg; // get next piece
        arg = strtok(NULL, " ");
    }

    // Debug
    if (DEBUG) {
        printf("bin %s\n", command.binary);
        for(int i = 0; i<=command.length ;i++){
            printf("arg: %s\n",command.arguments[i]);
        }
        printf("inputfile %s\n", command.input_file ? command.input_file : "NULL");
        printf("outputfile %s\n", command.output_file ? command.output_file : "NULL");
        printf("length %i\n", command.length);
    }
    
    return command;
}

// Executes a shell command
void ExecuteCommand(struct ShellCommand command) {
    // fork, exec, error handling (invalaid commands, errors, etc)
    if (strcmp(command.binary, "cd") == 0) {
        if (chdir(command.arguments[1]) == -1) {
            printf("Error %i (%s)\n", errno, strerror(errno));
        }
    } else {
        if (fork() == 0) {
            if (command.input_file) {
                FILE* infile = fopen(command.input_file, "r");
                dup2(fileno(infile), 0);
                fclose(infile); 
            }
            if (command.output_file) {
                FILE* outfile = fopen(command.output_file, "w");
                dup2(fileno(outfile), 1);
                fclose(outfile); 
            }
            execvp(command.binary, command.arguments);
            printf("Error %i (%s)\n", errno, strerror(errno));
            exit(1);
        } else wait(NULL);
    }
    // check errno
}

//* MAIN
int main() {
    char* input;
    struct ShellCommand command;
    for(;;) {
        input = CommandPrompt();
        command = ParseCommandLine(input);
        ExecuteCommand(command);
    }
    exit(0);
}