#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "utilities.h"

bool commandAndArgCompare(char *inputArray[], int index, const char string[])
{
    int charIndex = 0;

    if (inputArray[index] == NULL || string == NULL)
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

void printShellPrompt(void)
{
    char currentDirectory[PATH_BUFFER_SIZE];
    char *username = getenv("USER");
    char *folderName = NULL;
    int lastSlashIndex = 0;
    int charIndex = 0;

    if (username == NULL)
    {
        username = PROMPT_FALLBACK_USER;
    }

    if (getcwd(currentDirectory, sizeof(currentDirectory)) != NULL)
    {
        folderName = currentDirectory;

        while (currentDirectory[charIndex] != '\0')
        {
            if (currentDirectory[charIndex] == '/')
            {
                lastSlashIndex = charIndex;
            }

            charIndex = charIndex + 1;
        }

        if (currentDirectory[0] == '/' && currentDirectory[1] == '\0')
        {
            folderName = ROOT_FOLDER_NAME;
        }
        else if (currentDirectory[lastSlashIndex + 1] != '\0')
        {
            folderName = &currentDirectory[lastSlashIndex + 1];
        }

        printf("%s@%s$ ", username, folderName);
    }
    else
    {
        printf("%s$ ", username);
    }

    fflush(stdout);
}
