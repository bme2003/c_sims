#ifndef BUILTINS_H
#define BUILTINS_H

#include <stdbool.h>
#include "constants.h"

// Phase 2 builtins file plan
//
// Purpose:
// Keep built-in shell command behavior out of main.c so the shell loop stays small.
//
// Planned functions:
// bool handleBuiltInCommand(char *commandLineArgs[], bool *shellStatus);
// void printHelp(void);
// void printWorkingDirectory(void);
// void listDirectory(const char *path);
// void changeDirectory(const char *path);
// void clearScreen(void);
//
// Pseudocode for handleBuiltInCommand:
// 1. Check whether commandLineArgs[0] is NULL
// 2. Compare the command against each built-in command name
// 3. If a built-in matches, validate its arguments
// 4. Call the matching built-in helper
// 5. If the command was handled, return true
// 6. If the command was not handled, return false

#endif
