#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <dirent.h>
#include "utilities.h"


// Function prototypes:



int main(void)
{
    // Declare Variables
    // Create a character array to store the full line of user input
    // Create an array of character pointers to hold the command and its arguments
    // Create an integer for the child process ID
    // Create an integer to track the child process status
    // Create an integer or boolean to control whether the shell should keep running
    // Create any counters needed for splitting the input into tokens
    char userInput[100];
    char* commandLineArgs[40];
    //int childProcessId = 0;
    //int childProcessStatus;
    bool shellStatus = true;
    bool enteredNothing = false;
    int userCharItr;
    int commandLineArgsItr;
    int argNumber;


    // Logic
    // Start a loop that continues running until the user chooses to exit

    // Display a shell prompt so the user knows input is expected
    printf("Command Line Tool Active, for help type \"help\".\n\n\n\n");

    while (shellStatus)
    {
        // Read one full line of input from the keyboard
        printShellPrompt();
        if (fgets(userInput, 100, stdin) == NULL)
        {
            printf("No user input entered\n");
        }
        else
        {
            // Remove new line from end of user input
            removeNewLineAtEndOfCharArray(userInput);

            // Reset every argument pointer before parsing the next command
            for (commandLineArgsItr = 0; commandLineArgsItr < 40; commandLineArgsItr = commandLineArgsItr + 1)
            {
                commandLineArgs[commandLineArgsItr] = NULL;
            }

            // Split the input in place
            // Each time a word begins, store its address in commandLineArgs
            // Each time a separating space is found, replace it with '\0'
            // This turns one input line into many small strings inside userInput
            userCharItr = 0;
            argNumber = 0;

            while (userInput[userCharItr] != '\0' && argNumber < 39)
            {
                // Skip spaces between arguments
                while (userInput[userCharItr] == ' ')
                {
                    userCharItr = userCharItr + 1;
                }

                // Only store another word if there is still one to read
                if (userInput[userCharItr] != '\0')
                {
                    // Save the address of the start of the current word
                    commandLineArgs[argNumber] = &userInput[userCharItr];
                    argNumber = argNumber + 1;

                    // Move to the end of the current word
                    while (userInput[userCharItr] != '\0' && userInput[userCharItr] != ' ')
                    {
                        userCharItr = userCharItr + 1;
                    }

                    // End the current word and move to the next character
                    if (userInput[userCharItr] == ' ')
                    {
                        userInput[userCharItr] = '\0';
                        userCharItr = userCharItr + 1;
                    }
                }
            }

            // Add a NULL pointer at the end so later code knows where the args stop
            commandLineArgs[argNumber] = NULL;

            // If the user just pressed Enter, do nothing and let the loop ask again
            if (commandLineArgs[0] == NULL)
            {
                enteredNothing = true;
            }
            else
            {
                enteredNothing = false;
            }




            if (!enteredNothing && commandAndArgCompare(commandLineArgs, 0, "exit"))
            {
                printf("Exiting\n");
                shellStatus = false;
            }



            else if (!enteredNothing && commandAndArgCompare(commandLineArgs, 0, "help"))
            {
                printf("usage: command [arguments]\n");
            }



            else if (!enteredNothing && commandAndArgCompare(commandLineArgs, 0, "ls"))
            {
                // If the user did not provide a path, list the current directory
                // If the user did provide a path, use that argument as the directory path
                //printf("Listing...\n");

                if (commandLineArgs[1] == NULL)
                {
                    listDirectory(".");
                }
                else
                {
                    listDirectory(commandLineArgs[1]);
                }
            }

            else if (!enteredNothing && commandAndArgCompare(commandLineArgs, 0, "cd"))
            {
                if (commandLineArgs[1] != NULL)
                {
                    changeDirectory(commandLineArgs[1]);
                }
            }

            else if (!enteredNothing && commandAndArgCompare(commandLineArgs, 0, "clear"))
            {
                // how did someone find how to make this sequennce....
                printf("\033[2J\033[H");
                fflush(stdout);
            }

            else if (!enteredNothing)
            {
                // Let the loop end naturally, then fgets will ask for input again
                printf("Unknown command\n");
            }

            


        }
    }

    // returns
    // Return 0 when the shell program ends normally
    
    return 0;
}

