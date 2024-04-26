#ifndef PROGRAMS_H
#define PROGRAMS_H

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include <assert.h>
#include <fcntl.h>

// Redirection operators
#define INPUT_REDIRECTION '<'
#define OUTPUT_REDIRECTION '>'
#define READ 0
#define WRITE 1

void handleInputRedirection(char *fileName);
void handleOutputRedirection(char *fileName);
void redirectionCheck(char* currentCmd[], int count2);
void executeStdCmds(char* args[]);
int pipelineCheck(char* currentCmd[], int count2);
void executeOnePipeCmds(char* cmd1[], char* cmd2[]);
void executeTwoPipeCmds(char* cmd1[], char* cmd2[], char* cmd3[]);
void signalHandling();

#endif
