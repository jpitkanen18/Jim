#pragma once
#include <ncurses.h>

#include <fstream>
#include <sstream>
#include <vector>

#define INSERT_TEXT "INSERT"

struct vec2 {
    vec2() : x(0), y(0) {}
    int x, y;
};

enum Direction {
    UP,
    DOWN,
    LEFT,
    RIGHT
};

class Canvas {
    char **CanvasBuffer;
    char **DataBuffer;
    bool insertMode = false;
    std::vector<int> DataMallocced;

   public:
    int Columns;
    int Rows;
    int NumLines = 0;
    int OffsetX;
    int OffsetY;
    vec2 CursorLocation;

    Canvas(int col, int row, vec2 savedCursorLoc = vec2()) {
        Columns = col;
        Rows = row;
        CursorLocation = savedCursorLoc;
        CanvasBuffer = (char **)malloc(sizeof *CanvasBuffer * Rows);
        OffsetX = 0;
        OffsetY = 0;
        for (int i = 0; i < Rows; i++) {
            CanvasBuffer[i] = (char *)malloc(sizeof(char) * Columns - 1);
        }
        clearCanvasBuffer();
    }

    void loadFile(std::string filePath) {
        std::string line;
        std::ifstream inFile(filePath);

        if (inFile.is_open()) {
            while (!inFile.eof()) {
                std::getline(inFile, line);
                NumLines++;
            }
        }
        DataBuffer = (char **)malloc(sizeof *DataBuffer * (NumLines + 2));
        if (inFile.is_open()) {
            int i = 0;
            inFile.clear();
            inFile.seekg(0, std::ios::beg);
            while (!inFile.eof()) {
                std::getline(inFile, line);
                DataBuffer[i] = (char *)malloc(line.length() + 1);
                DataMallocced.push_back(line.length() + 1);
                strcpy(DataBuffer[i], line.data());
                i++;
            }
            inFile.close();
        }
    }

    void clearCanvasBuffer() {
        for (int i = 0; i < Rows; i++) {
            for (int j = 0; j < Columns - 1; j++) {
                CanvasBuffer[i][j] = '\0';
            }
            CanvasBuffer[i][0] = '~';
        }
    }

    void render() {
        clear();
        clearCanvasBuffer();
        for (int i = 0; i < Rows; i++) {
            if (i + OffsetY > NumLines - 2)
                break;
            strncpy(CanvasBuffer[i], DataBuffer[i + OffsetY], Columns - 2);
        }
        int reduce = 1;
        if (insertMode) {
            reduce = 2;
        }
        for (int i = 0; i < Rows - reduce; i++) {
            println(CanvasBuffer[i]);
        }
        if (insertMode) {
            println(INSERT_TEXT);
        }
        move(CursorLocation.y, CursorLocation.x);
        refresh();
    }

    void enterInsertMode() {
        insertMode = true;
    }

    void exitInsertMode() {
        insertMode = false;
    }

    bool getInsertMode() {
        return insertMode;
    }

    std::string removeAt(char *str, int idx) {
        std::string temp(str);
        temp.erase(idx, 1);
        return temp;
    }

    void mallocNewLine(int yPos, int size = CHAR_MAX) {
        DataBuffer[yPos] = (char *)malloc(size);
        DataMallocced.push_back(size);
    }

    void enterCharacter(int key) {
        int yPos = CursorLocation.y + OffsetY;
        int xPos = CursorLocation.x + OffsetX;
        if (key == 127) {
            if (xPos != 0 && yPos < DataMallocced.size()) {
                strcpy(DataBuffer[yPos], removeAt(DataBuffer[yPos], xPos - 1).data());
                moveCursor(LEFT);
            } else if (xPos == 0 && yPos > 0) {
                moveCursor(UP);
                int newYPos = CursorLocation.y + OffsetY;
                int oldLen = strlen(DataBuffer[newYPos]);
                if (strlen(DataBuffer[yPos]) > 0) {
                    DataBuffer[newYPos] = (char *)realloc(DataBuffer[newYPos], DataMallocced[newYPos] + DataMallocced[yPos] + 1);
                    DataMallocced[newYPos] = DataMallocced[newYPos] + DataMallocced[yPos] + 1;
                    char *tempBuf = (char *)malloc(DataMallocced[newYPos] + DataMallocced[yPos] + 1);
                    snprintf(tempBuf, DataMallocced[newYPos], "%s%s", DataBuffer[newYPos], DataBuffer[yPos]);
                    strcpy(DataBuffer[newYPos], tempBuf);
                    free(tempBuf);
                }
                free(DataBuffer[yPos]);
                memmove(DataBuffer + yPos, DataBuffer + yPos + 1, (--NumLines - yPos) * sizeof *DataBuffer);
                DataMallocced.erase(DataMallocced.begin() + yPos);
                if (oldLen > 0)
                    CursorLocation.x = strlen(DataBuffer[newYPos]);
            }
        } else if (key == 10) {
            int newYPos = CursorLocation.y + OffsetY + 1;
            if (newYPos >= DataMallocced.size()) {
                mallocNewLine(newYPos);
                CursorLocation.x = 0;
                moveCursor(DOWN);
            } else if (DataMallocced[newYPos] < xPos && xPos >= DataMallocced[yPos]) {
                if (xPos == 0) {
                    xPos = 1;
                }
                DataBuffer[newYPos] = (char *)realloc(NULL, xPos * 2);
                DataMallocced[newYPos] = xPos * 2;
                moveCursor(DOWN);
            } else if (CursorLocation.x < strlen(DataBuffer[yPos]) + 1) {
                std::string temp(DataBuffer[yPos]);
                DataBuffer = (char **)realloc(DataBuffer, sizeof *DataBuffer * (NumLines + 1));
                memmove(DataBuffer + newYPos, DataBuffer + newYPos - 1, (++NumLines - newYPos) * sizeof *DataBuffer);
                DataBuffer[newYPos] = (char *)realloc(NULL, temp.substr(xPos, temp.size() - 1).size() * 2);
                DataBuffer[yPos] = (char *)malloc(temp.substr(xPos, temp.size() - 1).size() * 2);
                strcpy(DataBuffer[yPos], temp.substr(0, xPos).data());
                strcpy(DataBuffer[newYPos], temp.substr(xPos, temp.size()).data());
                DataMallocced.insert(DataMallocced.begin() + newYPos, CHAR_MAX);
                CursorLocation.x = 0;
                moveCursor(DOWN);
            }
        } else if (key >= 32 && key <= 126) {
            if (yPos >= DataMallocced.size()) {
                mallocNewLine(yPos);
                DataBuffer[yPos][xPos] = char(key);
            } else {
                std::string temp(DataBuffer[yPos]);
                temp.insert(xPos, 1, char(key));
                if (strlen(DataBuffer[yPos]) + temp.size() > DataMallocced[yPos]) {
                    DataBuffer[yPos] = (char *)realloc(NULL, strlen(DataBuffer[yPos]) * 2);
                    DataMallocced[yPos] = strlen(DataBuffer[yPos]) * 2;
                }
                strcpy(DataBuffer[yPos], temp.data());
            }
            CursorLocation.x++;
        }
    }

    void moveCursor(Direction dir) {
        switch (dir) {
            case UP:
                --CursorLocation.y;
                if (CursorLocation.y < 0) {
                    CursorLocation.y = 0;
                    if (OffsetY > 0) {
                        OffsetY--;
                    }
                }
                if (strlen(DataBuffer[CursorLocation.y + OffsetY]) < CursorLocation.x) {
                    CursorLocation.x = strlen(DataBuffer[CursorLocation.y + OffsetY]);
                }
                break;
            case DOWN:
                if (CursorLocation.y + 1 <= NumLines - 2) {
                    CursorLocation.y++;
                }
                if (CursorLocation.y > Rows) {
                    CursorLocation.y = Rows;
                    OffsetY++;
                }
                if (CursorLocation.y + OffsetY <= NumLines - 2) {
                    if (strlen(DataBuffer[CursorLocation.y + OffsetY]) <= 0) {
                        CursorLocation.x = 0;
                    }
                }
                if (strlen(DataBuffer[CursorLocation.y + OffsetY]) < CursorLocation.x) {
                    CursorLocation.x = strlen(DataBuffer[CursorLocation.y + OffsetY]);
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
                if (strlen(DataBuffer[CursorLocation.y]) > CursorLocation.x)
                    CursorLocation.x++;
                if (CursorLocation.x > Columns) {
                    CursorLocation.x = Columns - 1;
                    if (OffsetX < DataMallocced[CursorLocation.y]) {
                        OffsetX++;
                    }
                }
                break;
        }
    }
};