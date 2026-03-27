#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "utilities.h"

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

void initializeCommandArguments(char *commandLineArgs[])
{
    int commandLineArgsItr = 0;

    for (commandLineArgsItr = 0; commandLineArgsItr < MAX_ARGUMENTS; commandLineArgsItr = commandLineArgsItr + 1)
    {
        commandLineArgs[commandLineArgsItr] = NULL;
    }
}

void tokenizeUserInput(char inputArray[], char *commandLineArgs[])
{
    int userCharItr = 0;
    int argNumber = 0;

    while (inputArray[userCharItr] != '\0' && argNumber < MAX_ARGUMENTS - 1)
    {
        while (inputArray[userCharItr] == ' ')
        {
            userCharItr = userCharItr + 1;
        }

        if (inputArray[userCharItr] != '\0')
        {
            commandLineArgs[argNumber] = &inputArray[userCharItr];
            argNumber = argNumber + 1;

            while (inputArray[userCharItr] != '\0' && inputArray[userCharItr] != ' ')
            {
                userCharItr = userCharItr + 1;
            }

            if (inputArray[userCharItr] == ' ')
            {
                inputArray[userCharItr] = '\0';
                userCharItr = userCharItr + 1;
            }
        }
    }

    commandLineArgs[argNumber] = NULL;
}

bool commandAndArgCompare(char *inputArray[], int index, const char string[])
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
        printf("%s", MESSAGE_BAD_PATH);
    }
    else
    {
        while ((entry = readdir(currentDirectory)) != NULL)
        {
            printf("%s    ", entry->d_name);
        }
        printf("\n");

        closedir(currentDirectory);
    }
}

void changeDirectory(const char *path)
{
    if (path == NULL)
    {
        printf("%s", MESSAGE_CD_PATH_REQUIRED);
    }
    else if (chdir(path) != 0)
    {
        printf("%s", MESSAGE_BAD_PATH);
    }
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

void printWorkingDirectory(void)
{
    char currentDirectory[PATH_BUFFER_SIZE];

    if (getcwd(currentDirectory, sizeof(currentDirectory)) != NULL)
    {
        printf("%s\n", currentDirectory);
    }
    else
    {
        printf("%s", MESSAGE_GETCWD_ERROR);
    }
}

void printHelp(void)
{
    printf("Built-in commands:\n");
    printf("  help   Show this help message\n");
    printf("  pwd    Print the current working directory\n");
    printf("  ls     List the current directory or a provided path\n");
    printf("  cd     Change the current directory\n");
    printf("  clear  Clear the screen\n");
    printf("  exit   Exit the shell\n");
}

void clearScreen(void)
{
    printf("\033[2J\033[H");
    fflush(stdout);
}
