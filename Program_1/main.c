#include <stdbool.h>
#include <stdio.h>
#include "utilities.h"


int main(void)
{
    // Vars
    char userInput[INPUT_BUFFER_SIZE];
    char *commandLineArgs[MAX_ARGUMENTS];
    bool shellStatus = true;
    bool enteredNothing = false;


    // Welcome print

    printf("%s", MESSAGE_WELCOME);

    while (shellStatus)
    {
        // user shell prompt: user folder
        printShellPrompt();


        // user input
        if (fgets(userInput, INPUT_BUFFER_SIZE, stdin) == NULL)
        {
            printf("%s", MESSAGE_INPUT_ERROR);
        }

        else
        {

            removeNewLineAtEndOfCharArray(userInput);
            initializeCommandArguments(commandLineArgs);
            tokenizeUserInput(userInput, commandLineArgs);

            if (commandLineArgs[0] == NULL)
            {
                enteredNothing = true;
            }

            else
            {
                enteredNothing = false;
            }

            if (!enteredNothing)
            {

                if (commandAndArgCompare(commandLineArgs, 0, COMMAND_EXIT))
                {
                    printf("%s", MESSAGE_EXITING);
                    shellStatus = false;
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
                

                }
                else
                {
                    printf("%s", MESSAGE_UNKNOWN_COMMAND);
                }
            }
        }
    }

    return 0;
}
