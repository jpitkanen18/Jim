#pragma once

#define WRITE_COMMAND 'w'
#define WRITE_COMMAND_CAPITALIZED 'W'
#define QUIT_COMMAND 'q'
#define QUIT_COMMAND_CAPITALIZED 'Q'

namespace Jim {
typedef enum {
    WRITE,
    QUIT,
} Command;

typedef struct {
    Command* commands;
    size_t size;
} Commands;

Commands* GetCommands(char* commandString) {
    size_t length = strlen(commandString);
    Command* temp = (Command*)malloc(sizeof(Command) * length);
    size_t actualSize = 0;

    for (size_t i = 0; i < length; i++) {
        char commandChar = commandString[i];
        if (commandChar == WRITE_COMMAND || commandChar == WRITE_COMMAND_CAPITALIZED) {
            temp[actualSize++] = WRITE;
        } else if (commandChar == QUIT_COMMAND || commandChar == QUIT_COMMAND_CAPITALIZED) {
            temp[actualSize++] = QUIT;
        }
    }

    Commands* commandsStruct = (Commands*)malloc(sizeof(Commands));
    commandsStruct->commands = (Command*)malloc(sizeof(Command) * actualSize);
    memcpy(commandsStruct->commands, temp, sizeof(Command) * actualSize);
    commandsStruct->size = actualSize;

    free(temp);
    return commandsStruct;
}

}  // namespace Jim