#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <dirent.h>


void removeNewLineAtEndOfCharArray(char inputArray[])
{
    int iter = 0;

    while (inputArray[iter] != '\0')
    {
        iter = iter + 1;
    }

    if (iter > 0 && inputArray[iter - 1] == '\n')
    {
        inputArray[iter - 1] = '\0';
    }
}

bool commandAndArgCompare(char* inputArray[], int index, char string[])
{
    int charIndex = 0;

    if (inputArray[index] == NULL)
    {
        return false;
    }

    while (inputArray[index][charIndex] != '\0' && string[charIndex] != '\0')
    {
        if (inputArray[index][charIndex] != string[charIndex])
        {
            return false;
        }
        charIndex = charIndex + 1;
    }

    if (inputArray[index][charIndex] == '\0' && string[charIndex] == '\0')
    {
        return true;
    }

    return false;
}

void listDirectory(const char *path)
{
    DIR *currentDirectory;
    struct dirent *entry;

    currentDirectory = opendir(path);

    if (currentDirectory == NULL)
    {
        printf("Bad path\n");
        return;
    }

    while ((entry = readdir(currentDirectory)) != NULL)
    {
        printf("%s    ", entry->d_name);
    }
    printf("\n");

    closedir(currentDirectory);
}
