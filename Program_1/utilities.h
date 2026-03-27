#ifndef UTILITIES_H
#define UTILITIES_H

#include <stdbool.h>
#include "constants.h"

void removeNewLineAtEndOfCharArray(char inputArray[]);
void initializeCommandArguments(char *commandLineArgs[]);
void tokenizeUserInput(char inputArray[], char *commandLineArgs[]);
bool commandAndArgCompare(char *inputArray[], int index, const char string[]);
void listDirectory(const char *path);
void changeDirectory(const char *path);
void printShellPrompt(void);
void printWorkingDirectory(void);
void printHelp(void);
void clearScreen(void);

#endif
