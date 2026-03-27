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


void changeDirectory(const char *path)
{
    if (path == NULL)
    {
        printf("cd requires a path\n");
    }
    else
    {
        if (chdir(path) != 0)
        {
            printf("Could not change dir, bad path.\n");
        }

    }
}

void printShellPrompt(void)
{
    char currentDirectory[1024];
    char *username = getenv("USER");
    char *folderName = NULL;
    int lastSlashIndex = 0;
    int charIndex = 0;

    if (username == NULL)
    {
        username = "user";
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
            folderName = currentDirectory;
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
