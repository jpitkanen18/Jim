#pragma once
#include <limits.h>
#include <ncurses.h>

#include <fstream>
#include <sstream>
#include <vector>

#define INSERT_TEXT "-- INSERT --"
#define COMMAND_TEXT ": "

namespace Jim {

typedef struct Character {
    char content[2];
} Character;

typedef struct Line {
    Character* chars;
    int length;
} Line;

typedef enum Direction {
    UP,
    DOWN,
    LEFT,
    RIGHT
} Direction;

}  // namespace Jim

#include "Commands.hpp"
#include "Utils.hpp"

namespace Jim {

struct Vec2 {
    Vec2() : x(0), y(0) {}
    int x, y;
};

class Canvas {
    Line* DataBuffer;
    Line* FrontBuffer;
    bool InsertMode = false;
    bool CommandPalette = false;
    char* CommandPaletteInput;
    int CommandPaletteLength = 0;
    std::string FilePath;

   public:
    int Columns;
    int Rows;
    int NumLines = 0;
    int OffsetX = 0;
    int OffsetY = 0;
    Vec2 CursorLocation;

    Canvas(int col, int row, Vec2 savedCursorLoc = Vec2()) {
        Columns = col;
        Rows = row;
        CursorLocation = savedCursorLoc;
        FrontBuffer = (Line*)malloc(sizeof *FrontBuffer * Rows);
        for (int i = 0; i < Rows; i++) {
            FrontBuffer[i].chars = (Character*)malloc(sizeof(Character) * Columns);
        }
        ClearFrontBuffer();
    }

    void ClearFrontBuffer() {
        Character* p = (Character*)malloc(sizeof(Character));
        strncpy(p->content, "~", 2);
        for (int i = 0; i < Rows; i++) {
            FrontBuffer[i].chars = p;
            FrontBuffer[i].length = 2;
        }
    }

    void CleanUpAndQuit() {
        // This ain't work?
        // for (int i = 0; i < Rows - 1; i++) {
        //     free(FrontBuffer[i].chars);
        // }
        free(FrontBuffer);
        // for (int i = 0; i < NumLines; i++) {
        //     free(DataBuffer[i].chars);
        // }
        free(DataBuffer);
        free(CommandPaletteInput);
        endwin();
        exit(EXIT_SUCCESS);
    }

    void HandleCommands() {
        char* terminatedInput = (char*)malloc(CommandPaletteLength + 1);
        memcpy(terminatedInput, CommandPaletteInput, CommandPaletteLength);
        terminatedInput[CommandPaletteLength] = '\0';
        Commands* commands = GetCommands(terminatedInput);
        free(terminatedInput);
        bool quit = false;
        bool write = false;
        for (int i = 0; i < commands->size; i++) {
            Command command = commands->commands[i];
            if (command == WRITE)
                write = true;
            if (command == QUIT)
                quit = true;
        }
        free(commands->commands);
        free(commands);
        if (write == true) {
            Utils::WriteToDisk(DataBuffer, NumLines, FilePath);
        }
        if (quit == true) {
            CleanUpAndQuit();
        }
    }

    bool
    IsInvalidCommandKey(int key) {
        return key != 127 && (char)key != 10 && (char)key != 'w' && (char)key != 'W' && (char)key != 'q' && (char)key != 'Q';
    }

    void PushToCommandPalette(int key) {
        if (IsInvalidCommandKey(key))
            return;
        if (key == 10) {
            HandleCommands();
        }
        if (key == 127) {
            if (CommandPaletteLength == 0) {
                return;
            }
            char* tempBuf = (char*)malloc((CommandPaletteLength - 1) * sizeof(char));
            if (tempBuf == nullptr) {
                return;
            }

            for (int i = 0; i < CommandPaletteLength - 1; i++) {
                tempBuf[i] = CommandPaletteInput[i];
            }
            free(CommandPaletteInput);
            CommandPaletteInput = tempBuf;
            CommandPaletteLength--;
        } else {
            if (CommandPaletteLength == 0) {
                CommandPaletteInput = (char*)malloc(sizeof(char));
            } else {
                char* tempBuf = (char*)realloc(CommandPaletteInput, (CommandPaletteLength + 1) * sizeof(char));
                if (tempBuf == nullptr) {
                    return;
                }
                CommandPaletteInput = tempBuf;
            }
            CommandPaletteInput[CommandPaletteLength] = (char)key;
            CommandPaletteLength++;
        }
    }

    Character* InitCharacter(const char* content) {
        Character* p = (Character*)malloc(sizeof(Character));
        strncpy(p->content, content, 1);
        return p;
    }

    void LoadFile(std::string filePath) {
        FilePath = filePath;
        std::string line;
        std::ifstream inFile(filePath);

        if (!inFile.is_open()) {
            std::cerr << "Error: Unable to open file!" << std::endl;
            return;
        }

        NumLines = 1;
        while (std::getline(inFile, line)) {
            NumLines++;
        }

        DataBuffer = (Line*)malloc(sizeof(Line) * NumLines);

        inFile.clear();
        inFile.seekg(0, std::ios::beg);

        int i = 0;
        while (std::getline(inFile, line)) {
            if (i > NumLines) break;

            size_t lineLength = line.length();

            if (lineLength == 0) {
                DataBuffer[i].length = 0;
                DataBuffer[i].chars = nullptr;
            } else {
                DataBuffer[i].length = lineLength;
                DataBuffer[i].chars = (Character*)malloc(sizeof(Character) * lineLength);

                for (size_t j = 0; j < lineLength; j++) {
                    Character* tempCharacter = InitCharacter(&line[j]);
                    DataBuffer[i].chars[j] = *tempCharacter;
                    free(tempCharacter);
                }
            }

            i++;
        }

        inFile.close();
    }

    void Render() {
        clear();
        ClearFrontBuffer();

        for (int i = 0; i < Rows + 1; i++) {
            if (i + OffsetY >= NumLines)
                break;

            FrontBuffer[i].length = DataBuffer[i + OffsetY].length;
            FrontBuffer[i].chars = (Character*)malloc(sizeof(Character) * FrontBuffer[i].length);

            if (FrontBuffer[i].chars != nullptr) {
                memcpy(FrontBuffer[i].chars, DataBuffer[i + OffsetY].chars, sizeof(Character) * FrontBuffer[i].length);
            }
        }

        int reduce = (InsertMode || CommandPalette) ? 2 : 1;

        for (int i = 0; i < Rows - reduce; i++) {
            if (FrontBuffer[i].length == 0 || FrontBuffer[i].chars == nullptr) {
                Utils::Println(" ");
            } else {
                Utils::Println(&FrontBuffer[i]);
            }
        }

        if (InsertMode) {
            Utils::Println(INSERT_TEXT);
        } else if (CommandPalette) {
            Utils::Print(COMMAND_TEXT);
            if (CommandPaletteLength > 0)
                Utils::Println(CommandPaletteInput);
            else
                Utils::Println("");
        }

        move(CursorLocation.y, CursorLocation.x);
        refresh();
    }

    void EnterInsertMode() {
        InsertMode = true;
    }

    void EnterCommandPalette() {
        CommandPalette = true;
    }

    void ExitModes() {
        if (InsertMode)
            InsertMode = false;
        if (CommandPalette)
            CommandPalette = false;
    }

    bool GetInsertMode() {
        return InsertMode;
    }

    bool GetCommandPalette() {
        return CommandPalette;
    }

    void HandleBackspace(int xPos, int yPos) {
        Line* currentLine = &DataBuffer[yPos];
        if (xPos > 0 && yPos < NumLines) {
            if (currentLine->length > 0) {
                for (int i = xPos - 1; i < currentLine->length - 1; i++) {
                    currentLine->chars[i] = currentLine->chars[i + 1];
                }
                currentLine->length--;
                Character* newBuf = (Character*)realloc(currentLine->chars, currentLine->length * sizeof(Character));
                if (newBuf || currentLine->length == 0) {
                    currentLine->chars = newBuf;
                }
                MoveCursor(LEFT);
            }
        } else if (xPos == 0 && yPos > 0) {
            Line* prevLine = &DataBuffer[yPos - 1];
            int originalLength = prevLine->length;
            if (currentLine->length > 0) {
                Character* newBuf = (Character*)realloc(
                    prevLine->chars,
                    (prevLine->length + currentLine->length) * sizeof(Character));

                if (newBuf) {
                    prevLine->chars = newBuf;
                    memcpy(&prevLine->chars[prevLine->length], currentLine->chars, currentLine->length * sizeof(Character));
                    prevLine->length += currentLine->length;
                }
            }
            free(currentLine->chars);

            if (NumLines > 0) {
                for (int i = yPos; i < NumLines - 1; i++) {
                    DataBuffer[i] = DataBuffer[i + 1];
                }
            }

            NumLines--;

            Line* newDataBuf = (Line*)realloc(DataBuffer, sizeof(Line) * NumLines);
            if (newDataBuf || NumLines == 0) {
                DataBuffer = newDataBuf;
            }

            CursorLocation.x = originalLength;
            CursorLocation.y = CursorLocation.y - 1;
        }
    }

    void HandleEnter(int xPos, int yPos) {
        Line* currentLine = &DataBuffer[yPos];
        Line* newDataBuf = (Line*)malloc(sizeof(Line) * (NumLines + 1));
        if (NumLines > 0) {
            for (int i = 0; i < yPos + 1; i++) {
                newDataBuf[i] = DataBuffer[i];
            }
            for (int i = yPos + 2; i < NumLines + 1; i++) {
                newDataBuf[i] = DataBuffer[i - 1];
            }

            if (xPos == 0) {
                newDataBuf[yPos] = *(Line*)malloc(sizeof(Line));
                newDataBuf[yPos + 1] = *currentLine;
            } else if (currentLine->length == 0) {
                newDataBuf[yPos + 1] = *(Line*)malloc(sizeof(Line));
            } else {
                int currentLengthOrig = currentLine->length;
                Line* newLine = (Line*)malloc(sizeof(Line));
                newLine->chars = (Character*)malloc((currentLengthOrig - xPos) * sizeof(Character));
                int newLength = 0;
                for (int i = xPos; i < currentLengthOrig; i++) {
                    newLine->chars[newLength] = currentLine->chars[i];
                    newLength++;
                }
                currentLine->chars = (Character*)realloc(currentLine->chars, (currentLengthOrig - newLength) * sizeof(Character));
                currentLine->length = currentLengthOrig - newLength;
                newLine->length = newLength;
                newDataBuf[yPos] = *currentLine;
                newDataBuf[yPos + 1] = *newLine;
            }
        }

        free(DataBuffer);
        DataBuffer = newDataBuf;

        NumLines++;

        CursorLocation.x = 0;
        CursorLocation.y = CursorLocation.y + 1;
    }

    void HandleCharacter(int xPos, int yPos, int key) {
        Line* currentLine = &DataBuffer[yPos];
        if (currentLine == nullptr || currentLine->length == 0) {
            if (NumLines == 0)
                DataBuffer = (Line*)realloc(DataBuffer, 1 * sizeof(Line));
            currentLine->chars = (Character*)malloc(1 * sizeof(Character));
            Character* newChar = InitCharacter((const char*)&key);
            currentLine->chars[0] = *newChar;
            currentLine->length = 1;
            free(newChar);
            MoveCursor(RIGHT);
            return;
        }
        Character* newChar = InitCharacter((const char*)&key);
        Character* newBuf = (Character*)malloc((currentLine->length + 1) * sizeof(Character));
        if (newBuf) {
            for (int i = 0; i < xPos; i++) {
                newBuf[i] = currentLine->chars[i];
            }
            for (int i = xPos + 1; i < currentLine->length + 1; i++) {
                newBuf[i] = currentLine->chars[i - 1];
            }
            newBuf[xPos] = *newChar;
        }
        free(currentLine->chars);
        currentLine->chars = newBuf;
        free(newChar);
        currentLine->length++;
        MoveCursor(RIGHT);
    }

    void EnterCharacter(int key) {
        int xPos = CursorLocation.x + OffsetX;
        int yPos = CursorLocation.y + OffsetY;
        if (key == 127) {
            HandleBackspace(xPos, yPos);
        } else if (key == 10) {
            HandleEnter(xPos, yPos);
        } else if (key >= 32 && key <= 126) {
            HandleCharacter(xPos, yPos, key);
        }
    }

    void MoveCursor(Direction dir) {
        switch (dir) {
            case UP:
                --CursorLocation.y;
                if (CursorLocation.y < 0) {
                    CursorLocation.y = 0;
                    if (OffsetY > 0) {
                        OffsetY--;
                    }
                }
                if (DataBuffer[CursorLocation.y + OffsetY].length < CursorLocation.x) {
                    CursorLocation.x = DataBuffer[CursorLocation.y + OffsetY].length;
                }
                break;
            case DOWN:
                if (CursorLocation.y <= NumLines - 2) {
                    CursorLocation.y++;
                }
                if (CursorLocation.y > Rows) {
                    CursorLocation.y = Rows - 1;
                    if (NumLines > Rows)
                        OffsetY++;
                }
                if (CursorLocation.y + OffsetY <= NumLines - 2) {
                    if (DataBuffer[CursorLocation.y + OffsetY].length <= 0) {
                        CursorLocation.x = 0;
                    }
                }
                if (DataBuffer[CursorLocation.y + OffsetY].length < CursorLocation.x) {
                    CursorLocation.x = DataBuffer[CursorLocation.y + OffsetY].length;
                }
                break;
            case LEFT:
                --CursorLocation.x;
                if (CursorLocation.x < 0) {
                    CursorLocation.x = 0;
                    if (OffsetX > 0) {
                        OffsetX--;
                    }
                }
                break;
            case RIGHT:
                if (DataBuffer[CursorLocation.y].length > CursorLocation.x)
                    CursorLocation.x++;
                if (CursorLocation.x > Columns) {
                    CursorLocation.x = Columns - 1;
                    if (OffsetX < DataBuffer[CursorLocation.y].length) {
                        OffsetX++;
                    }
                }
                break;
        }
    }
};
}  // namespace Jim