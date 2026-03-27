#include <stdbool.h>
#include <stdio.h>
#include "parser.h"
#include "utilities.h"
#include "builtins.h"

int main(void)
{
    char userInput[INPUT_BUFFER_SIZE];
    char *commandLineArgs[MAX_ARGUMENTS];
    bool shellStatus = true;
    bool enteredNothing = false;

    printf("%s", MESSAGE_WELCOME);

    while (shellStatus)
    {
        printShellPrompt();

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
                handleBuiltInCommand(commandLineArgs, &shellStatus);
            }
        }
    }

    return 0;
}
