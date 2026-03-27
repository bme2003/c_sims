#ifndef PARSER_H
#define PARSER_H

#include "constants.h"

// Phase 2 parser file plan
//
// Purpose:
// Keep all input-cleaning and tokenizing responsibilities here so main.c
// does not need to know the parsing details.
//
// Planned functions:
// void removeNewLineAtEndOfCharArray(char inputArray[]);
// void initializeCommandArguments(char *commandLineArgs[]);
// void tokenizeUserInput(char inputArray[], char *commandLineArgs[]);
//
// Pseudocode for removeNewLineAtEndOfCharArray:
// 1. Start at index 0
// 2. Move forward until the null terminator is found
// 3. If the previous character is a newline, replace it with a null terminator
//
// Pseudocode for initializeCommandArguments:
// 1. Loop through every argument slot
// 2. Set each slot to NULL
//
// Pseudocode for tokenizeUserInput:
// 1. Start at the beginning of the input array
// 2. Skip leading spaces
// 3. When a word starts, store its address in commandLineArgs
// 4. Move forward until a space or null terminator is found
// 5. If a space is found, replace it with a null terminator
// 6. Move to the next character and repeat
// 7. End the argument array with NULL

#endif
