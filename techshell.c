//* Techshell - Aiden Plauche, Cullen Duval - CSC 222 001 - Winter 2022-2023 *//
#include <stdio.h>

// Displays cwd and returns user input
char* CommandPrompt() {
    // "pwd$ ", scanf
}

// Wrapper for shell command
struct ShellCommand {
    char* binary; // the binary to execute (e.g., ls);
    char** arguments; // the arguments to send to the binary;
    // additional fields for i/o redirection
};

// Processes the user input as a Shell Command
struct ShellCommand ParseCommandLine(char* input) {
    // string parsing
    // check for i/o redirection first (< in, > out)
}

// Executes a shell command
void ExecuteCommand(struct ShellCommand command) {
    // fork, exec, error handling (invalaid commands, errors, etc)
}

/* redirect stdin from a file snippet
FILE* infile = fopen(my_input_file, "r");
dup2(fileno(infile), 0);
fclose(infile); 
*/

/* redirect stdout to a file snippet
FILE* outfile = fopen(my_output_file, "w");
dup2(fileno(outfile), 1);
fclose(outfile); 
*/

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