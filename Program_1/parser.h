#ifndef PARSER_H
#define PARSER_H

#include "constants.h"

void removeNewLineAtEndOfCharArray(char inputArray[]);
void initializeCommandArguments(char *commandLineArgs[]);
void tokenizeUserInput(char inputArray[], char *commandLineArgs[]);

#endif
