#ifndef UTILITIES_H
#define UTILITIES_H

#include <stdio.h>
#include <string.h>

#define BUFFER_MAX 512

void welcomeMessage();
void cmdPrompt();
void cmdLineReader(char cmdBuffer[]);

#endif
