#include "parser.h"
#include <stddef.h>

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
