#include <dirent.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>
#include "builtins.h"
#include "utilities.h"

typedef bool (*BuiltInHandler)(char *commandLineArgs[], bool *shellStatus);

typedef struct BuiltInCommand
{
    const char *name;
    BuiltInHandler handler;
} BuiltInCommand;

static bool handleExitCommand(char *commandLineArgs[], bool *shellStatus);
static bool handleHelpCommand(char *commandLineArgs[], bool *shellStatus);
static bool handlePwdCommand(char *commandLineArgs[], bool *shellStatus);
static bool handleLsCommand(char *commandLineArgs[], bool *shellStatus);
static bool handleCdCommand(char *commandLineArgs[], bool *shellStatus);
static bool handleClearCommand(char *commandLineArgs[], bool *shellStatus);
static bool handleTouchCommand(char *commandLineArgs[], bool *shellStatus);
static bool handleRmCommand(char *commandLineArgs[], bool *shellStatus);
static bool hasNoExtraArguments(char *commandLineArgs[]);
static bool hasExactlyOneArgument(char *commandLineArgs[]);

bool handleBuiltInCommand(char *commandLineArgs[], bool *shellStatus)
{
    BuiltInCommand builtInCommands[] = {
        {COMMAND_EXIT, handleExitCommand},
        {COMMAND_HELP, handleHelpCommand},
        {COMMAND_PWD, handlePwdCommand},
        {COMMAND_LS, handleLsCommand},
        {COMMAND_CD, handleCdCommand},
        {COMMAND_CLEAR, handleClearCommand},
        {COMMAND_TOUCH, handleTouchCommand},
        {COMMAND_RM, handleRmCommand}
        //ADD MORE HERE
    };
    int commandCount = (int)(sizeof(builtInCommands) / sizeof(builtInCommands[0]));
    int commandIndex = 0;
    bool commandHandled = false;

    while (commandIndex < commandCount)
    {
        if (!commandHandled && commandAndArgCompare(commandLineArgs, 0, builtInCommands[commandIndex].name))
        {
            commandHandled = builtInCommands[commandIndex].handler(commandLineArgs, shellStatus);
        }

        commandIndex = commandIndex + 1;
    }

    if (!commandHandled)
    {
        printf("%s", MESSAGE_UNKNOWN_COMMAND);
    }

    return commandHandled;
}

static bool handleExitCommand(char *commandLineArgs[], bool *shellStatus)
{
    if (hasNoExtraArguments(commandLineArgs))
    {
        printf("%s", MESSAGE_EXITING);
        *shellStatus = false;
    }
    else
    {
        printf("usage: exit\n");
    }

    return true;
}

static bool handleHelpCommand(char *commandLineArgs[], bool *shellStatus)
{
    (void)shellStatus;
    if (hasNoExtraArguments(commandLineArgs))
    {
        printHelp();
    }
    else
    {
        printf("%s", MESSAGE_HELP_USAGE);
    }

    return true;
}

static bool handlePwdCommand(char *commandLineArgs[], bool *shellStatus)
{
    (void)shellStatus;
    if (hasNoExtraArguments(commandLineArgs))
    {
        printWorkingDirectory();
    }
    else
    {
        printf("%s", MESSAGE_PWD_USAGE);
    }

    return true;
}

static bool handleLsCommand(char *commandLineArgs[], bool *shellStatus)
{
    (void)shellStatus;
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

static bool handleCdCommand(char *commandLineArgs[], bool *shellStatus)
{
    (void)shellStatus;
    if (hasExactlyOneArgument(commandLineArgs))
    {
        changeDirectory(commandLineArgs[1]);
    }
    else
    {
        printf("%s", MESSAGE_CD_PATH_REQUIRED);
    }

    return true;
}

static bool handleClearCommand(char *commandLineArgs[], bool *shellStatus)
{
    (void)shellStatus;
    if (hasNoExtraArguments(commandLineArgs))
    {
        clearScreen();
    }
    else
    {
        printf("%s", MESSAGE_CLEAR_USAGE);
    }

    return true;
}

static bool handleTouchCommand(char *commandLineArgs[], bool *shellStatus)
{
    (void)shellStatus;
    if (hasExactlyOneArgument(commandLineArgs))
    {
        createFile(commandLineArgs[1]);
    }
    else
    {
        printf("%s", MESSAGE_TOUCH_USAGE);
    }

    return true;
}

static bool handleRmCommand(char *commandLineArgs[], bool *shellStatus)
{
    (void)shellStatus;
    if (hasExactlyOneArgument(commandLineArgs))
    {
        deleteFile(commandLineArgs[1]);
    }
    else
    {
        printf("%s", MESSAGE_RM_USAGE);
    }

    return true;
}

static bool hasNoExtraArguments(char *commandLineArgs[])
{
    if (commandLineArgs[1] == NULL)
    {
        return true;
    }

    return false;
}

static bool hasExactlyOneArgument(char *commandLineArgs[])
{
    if (commandLineArgs[1] != NULL && commandLineArgs[2] == NULL)
    {
        return true;
    }

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
    printf("  touch  Create an empty file\n");
    printf("  rm     Delete a file\n");
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

void createFile(const char *path)
{
    FILE *filePointer;

    if (path == NULL)
    {
        printf("%s", MESSAGE_TOUCH_USAGE);
    }
    else
    {
        filePointer = fopen(path, "w");

        if (filePointer == NULL)
        {
            printf("%s", MESSAGE_FILE_CREATE_ERROR);
        }
        else
        {
            fclose(filePointer);
        }
    }
}

void deleteFile(const char *path)
{
    if (path == NULL)
    {
        printf("%s", MESSAGE_RM_USAGE);
    }
    else if (remove(path) != 0)
    {
        printf("%s", MESSAGE_FILE_DELETE_ERROR);
    }
}
