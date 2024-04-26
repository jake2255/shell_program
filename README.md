Major 2 for group 20 CSCE 3600

Made by Jacob Ulloa, Giancarlo Ramirez, and Adam Smith

Organization:

Jacob- Exit built-in, pipelining extension, text-parsing

Giancarlo- CD and path built-in, I/O redirection

Design Overview:
The main.c file contains the main function, which essentially starts the batch and interactive modes. The batch mode is optional, and when the batch mode is completed it runs in interactive mode. The utilities.h and utilities.c files contains some miscellanous functions, the primary function being the cmdLineReader which reads in the buffer from the command line and parses the data into arrays. It also calls the neccessary function to run the commands. This function also calls the i/o redirection and pipelining functions and implements the cd builtin command. The builtin.h and builtin.c files contains functions to implement the exit and path built in commands. The programs.h and programs.c files contain the implementation for the i/o redirection, pipelining, and command execution. The redirectionCheck and pipelineCheck functions check whether redirection and pipeling are neccesary for the current commands. If they find that it is neccesary, the handleInputRedirection and handleOutputRedirection functions are called to implement i/o redirection and the executeOnePipeCmds and executeTwoPipeCmds functions are called if either one or two pipes are needed. The executeStdCmds execute standard commands that dont require redirection or pipeling.

Complete Specification:
To deal with commands lines that are empty, the cmdLineReader function parses the data based on whitespace and semicolons and store the data in arrays. When the whitespace/semicolons are removed, if the resulting array buffer is null, that means that no arguments were typed in the command line and allows the shell to "eat" the input.

Known Bugs:

Pipelining will sometimes not work, likely a parsing/memory issue.
