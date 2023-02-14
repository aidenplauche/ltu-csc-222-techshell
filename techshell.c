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
    char* input = malloc(sizeof(char) * 256); // reserve space for input
    char cwd[256];
    getcwd(cwd, sizeof(cwd)); // get and store the current working directory
    printf("%s$ ", cwd); // print command line prompt
    
    fgets(input, 256, stdin); // get user input
    input[strlen(input) - 1] = '\0'; // replace newline with null

    if (strcmp(input, "exit") == 0) exit(0); // exit builtin command

    return input;
}

// Wrapper for shell command
struct ShellCommand {
    char* binary; // the binary to execute (e.g., ls);
    char* arguments[256]; // the arguments to send to the binary;
    char* input_file; // file to redirect stdin to
    char* output_file; // file to redirect stdout to
};

// Processes the user input as a Shell Command
struct ShellCommand ParseCommandLine(char* input) {
    struct ShellCommand command = {NULL, {""}, NULL, NULL}; // initializes shellCommand structure

    // stores the arguments from input into the structure variable arguments
    int argNum = 0;
    command.arguments[argNum] = strtok(input, " "); // split input by spaces
    command.binary = command.arguments[argNum]; // sets binary to first argument

    char* arg = strtok(NULL, " "); // gets next piece
    while (arg != NULL) { // last argument from strtok will be null
        if (strcmp(arg, "<") == 0) { // checks for input redirection
            command.input_file = strtok(NULL, " "); // saves file name (next piece)
            if (!command.input_file) break; // breaks for no input file name case
            arg = strtok(NULL, " "); // gets next piece, must be > or isn't valid
            if (!arg) break;
            else if (strcmp(arg, ">") != 0) break;
        } 
        if (strcmp(arg, ">") == 0) { // checks for output redirection
            command.output_file = strtok(NULL, " "); // saves file name (next piece)
            break; // nothing should come after output redirection
        }
        
        if (arg[0] == '$')
            arg = getenv(&arg[1]);
        
        command.arguments[++argNum] = arg; // saves this argument
        arg = strtok(NULL, " "); // gets the next argument
    }

    // Debug print (contents of command struct and number of arguments)
    if (DEBUG) {
        printf("bin %s\n", command.binary);
        for (int i = 0; i <= argNum; i++){
            printf("arg%i %s\n", i, command.arguments[i]);
        }
        printf("inputfile %s\n", command.input_file ? command.input_file : "NULL");
        printf("outputfile %s\n", command.output_file ? command.output_file : "NULL");
        printf("number of args %i\n", argNum);
    }
    
    return command;
}

// Executes a shell command
void ExecuteCommand(struct ShellCommand command) {
    if (!command.binary) return; // if nothing was entered, don't execute a command
    if (strcmp(command.binary, "cd") == 0) { // change directory builtin command
        if (chdir(command.arguments[1]) == -1) { // if failed, print errno/string
            printf("Error %i (%s)\n", errno, strerror(errno));
        }
    } else {
        if (fork() == 0) { // create child process
            if (command.input_file) { // if input file specified, redirect stdin
                FILE* infile = fopen(command.input_file, "r");
                dup2(fileno(infile), 0);
                fclose(infile); 
            }
            if (command.output_file) { // if output file specified, redirect stdout
                FILE* outfile = fopen(command.output_file, "w");
                dup2(fileno(outfile), 1);
                fclose(outfile); 
            }
            execvp(command.binary, command.arguments); // execute specified command with arguments
            printf("Error %i (%s)\n", errno, strerror(errno)); // if return, error, print errno/string
            exit(1); // exit with error
        } else wait(NULL); // wait for child to terminate
    }
}

//* MAIN
int main() {
    char* input;
    struct ShellCommand command;

    // repeatedly prompt the user for input
    for(;;) {
        input = CommandPrompt();

        // parse the command line
        command = ParseCommandLine(input);

        // execute the command
        ExecuteCommand(command);
    }
    exit(0);
}