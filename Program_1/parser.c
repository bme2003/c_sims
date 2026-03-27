#include "parser.h"

// Phase 2 parser implementation notes
//
// This file is intended to hold the real parsing code later.
// For now it only documents the structure.
//
// Planned implementation order:
// 1. Move removeNewLineAtEndOfCharArray here from utilities.c
// 2. Move initializeCommandArguments here from utilities.c
// 3. Move tokenizeUserInput here from utilities.c
// 4. Keep parsing-only work here and avoid putting command behavior here
//
// Function pseudocode:
// removeNewLineAtEndOfCharArray:
// - Walk through the character array until '\0'
// - If the last real character is '\n', replace it with '\0'


//
// initializeCommandArguments:
// - Use a for loop from 0 up to MAX_ARGUMENTS
// - Set each commandLineArgs entry to NULL
//
// tokenizeUserInput:
// - Use one index for characters in the input
// - Use one index for which argument number is being filled
// - Skip spaces before each token
// - Save the start address of each token
// - Move to the end of the token
// - Replace separating spaces with '\0'
// - Stop when '\0' is reached or the argument limit is reached
