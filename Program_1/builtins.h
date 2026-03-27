#ifndef BUILTINS_H
#define BUILTINS_H

#include <stdbool.h>
#include "constants.h"

bool handleBuiltInCommand(char *commandLineArgs[], bool *shellStatus);
void printHelp(void);
void printWorkingDirectory(void);
void listDirectory(const char *path);
void changeDirectory(const char *path);
void clearScreen(void);
void createFile(const char *path);
void deleteFile(const char *path);

#endif
