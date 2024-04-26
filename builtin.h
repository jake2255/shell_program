#ifndef BUILTIN_H
#define BUILTIN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void handleExitCmd();
void displayCurrentPath();
void appPath(const char *pathname);
void remPath(const char *pathname);
void storeOriginalPath();
void restoreOriginalPath();

#endif
