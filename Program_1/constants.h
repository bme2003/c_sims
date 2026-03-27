#ifndef CONSTANTS_H
#define CONSTANTS_H

// SIZES
#define INPUT_BUFFER_SIZE 100
#define MAX_ARGUMENTS 40
#define PATH_BUFFER_SIZE 1024

// PROMPT/FOLDER
#define PROMPT_FALLBACK_USER "user"
#define ROOT_FOLDER_NAME "/"

// COMMANDS
#define COMMAND_EXIT "exit"
#define COMMAND_HELP "help"
#define COMMAND_LS "ls"
#define COMMAND_CD "cd"
#define COMMAND_CLEAR "clear"
#define COMMAND_PWD "pwd"
#define COMMAND_TOUCH "touch"
#define COMMAND_RM "rm"

// DIRS
#define DEFAULT_LIST_DIRECTORY "."

// MESSAGES
#define MESSAGE_WELCOME "Command Line Tool Active, for help type \"help\".\n"
#define MESSAGE_INPUT_ERROR "No user input entered\n"
#define MESSAGE_UNKNOWN_COMMAND "Unknown command\n"
#define MESSAGE_EXITING "Exiting\n"
#define MESSAGE_BAD_PATH "Bad path\n"
#define MESSAGE_CD_PATH_REQUIRED "cd requires exactly one path\n"
#define MESSAGE_LS_USAGE "usage: ls [path]\n"
#define MESSAGE_HELP_USAGE "usage: help\n"
#define MESSAGE_PWD_USAGE "usage: pwd\n"
#define MESSAGE_CLEAR_USAGE "usage: clear\n"
#define MESSAGE_TOUCH_USAGE "usage: touch <file>\n"
#define MESSAGE_RM_USAGE "usage: rm <file>\n"
#define MESSAGE_GETCWD_ERROR "Could not get current directory\n"
#define MESSAGE_FILE_CREATE_ERROR "Could not create file\n"
#define MESSAGE_FILE_DELETE_ERROR "Could not delete file\n"

#endif
