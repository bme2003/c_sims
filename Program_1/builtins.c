#include <dirent.h>
#include <stdio.h>
#include <unistd.h>
#include "builtins.h"
#include "utilities.h"

bool handleBuiltInCommand(char *commandLineArgs[], bool *shellStatus)
{
    if (commandLineArgs[0] == NULL)
    {
        return false;
    }

    if (commandAndArgCompare(commandLineArgs, 0, COMMAND_EXIT))
    {
        printf("%s", MESSAGE_EXITING);
        *shellStatus = false;
        return true;
    }
    else if (commandAndArgCompare(commandLineArgs, 0, COMMAND_HELP))
    {
        if (commandLineArgs[1] == NULL)
        {
            printHelp();
        }
        else
        {
            printf("%s", MESSAGE_HELP_USAGE);
        }
        return true;
    }
    else if (commandAndArgCompare(commandLineArgs, 0, COMMAND_PWD))
    {
        if (commandLineArgs[1] == NULL)
        {
            printWorkingDirectory();
        }
        else
        {
            printf("%s", MESSAGE_PWD_USAGE);
        }
        return true;
    }
    else if (commandAndArgCompare(commandLineArgs, 0, COMMAND_LS))
    {
        if (commandLineArgs[1] == NULL)
        {
            listDirectory(DEFAULT_LIST_DIRECTORY);
        }
        else if (commandLineArgs[2] == NULL)
        {
            listDirectory(commandLineArgs[1]);
        }
        else
        {
            printf("%s", MESSAGE_LS_USAGE);
        }
        return true;
    }
    else if (commandAndArgCompare(commandLineArgs, 0, COMMAND_CD))
    {
        if (commandLineArgs[1] != NULL && commandLineArgs[2] == NULL)
        {
            changeDirectory(commandLineArgs[1]);
        }
        else
        {
            printf("%s", MESSAGE_CD_PATH_REQUIRED);
        }
        return true;
    }
    else if (commandAndArgCompare(commandLineArgs, 0, COMMAND_CLEAR))
    {
        if (commandLineArgs[1] == NULL)
        {
            clearScreen();
        }
        else
        {
            printf("%s", MESSAGE_CLEAR_USAGE);
        }
        return true;
    }

    printf("%s", MESSAGE_UNKNOWN_COMMAND);
    return false;
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

void clearScreen(void)
{
    printf("\033[2J\033[H");
    fflush(stdout);
}
