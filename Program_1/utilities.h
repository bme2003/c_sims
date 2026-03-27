#ifndef UTILITIES_H
#define UTILITIES_H

#include <stdbool.h>
#include "constants.h"

bool commandAndArgCompare(char *inputArray[], int index, const char string[]);
void printShellPrompt(void);

#endif
