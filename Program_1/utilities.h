#ifndef UTILITIES_H
#define UTILITIES_H


#include <stdbool.h>


void removeNewLineAtEndOfCharArray(char inputArray[]);
bool commandAndArgCompare(char* inputArray[], int index, char string[]);
void listDirectory(const char *path);

#endif