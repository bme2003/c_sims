#include "builtins.h"

// Phase 2 builtins implementation notes
//
// This file is intended to hold built-in command execution later.
// For now it only documents the structure.
//
// Planned implementation order:
// 1. Move printHelp here from utilities.c
// 2. Move printWorkingDirectory here from utilities.c
// 3. Move listDirectory here from utilities.c
// 4. Move changeDirectory here from utilities.c
// 5. Move clearScreen here from utilities.c
// 6. Add a handleBuiltInCommand dispatcher here
//
// Function pseudocode:
// handleBuiltInCommand:
// - If no command exists, return false
// - If command is exit:
//   set shellStatus to false
//   print the exit message
//   return true
// - Else if command is help:
//   validate arguments
//   print help output
//   return true
// - Else if command is pwd:
//   validate arguments
//   print working directory
//   return true
// - Else if command is ls:
//   validate arguments
//   list the current directory or the provided path
//   return true
// - Else if command is cd:
//   validate arguments
//   try to change directory
//   return true
// - Else if command is clear:
//   validate arguments
//   clear the screen
//   return true
// - Else:
//   return false
//
// printHelp:
// - Print each supported built-in command and a short description
//
// printWorkingDirectory:
// - Ask the OS for the current working directory
// - Print it if successful
// - Print an error message if it fails
//
// listDirectory:
// - Open the target directory
// - If opening fails, print an error
// - Otherwise loop through each entry and print its name
// - Close the directory when finished
//
// changeDirectory:
// - If no path was provided, print an error
// - Otherwise call chdir on the path
// - If chdir fails, print an error
//
// clearScreen:
// - Print the terminal escape sequence to clear the screen
// - Flush stdout so the terminal updates immediately
