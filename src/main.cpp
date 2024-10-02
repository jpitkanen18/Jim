#include <ncurses.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include <cstring>
#include <fstream>
#include <sstream>
#include <string>

#include "canvas.h"
#include "input.h"
#include "utils.h"

static bool debug = true;

bool test(char *filePath) {
    std::ifstream test(filePath);
    if (!test) {
        if (debug) {
            println("The file doesn't exist");
        }
        return false;
    } else {
        if (debug) {
            print("File found at: ");
            print(filePath);
            println(" :)");
        }
        return true;
    }
}

std::stringstream fileContents(char *filePath) {
    std::ifstream inFile;
    inFile.open(filePath);
    std::stringstream strStream;
    strStream << inFile.rdbuf();
    return strStream;
}

int main(int argc, char **argv) {
    setlocale(LC_ALL, "");
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    scrollok(stdscr, TRUE);
    printMetadata();
    if (debug) {
        printArgs(argc, argv);
    }
    struct winsize size;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &size);
    print("ws_col: ");
    println(size.ws_col);
    print("ws_row: ");
    println(size.ws_row);
    if (argc == 1) {
        if (debug) {
            println("No file provided :(");
        }
        return 1;
    }
    char *filePath = argv[1];
    if (!test(filePath)) {
        println("Invalid file path, exiting now :/");
        return 1;
    }

    Canvas canvas(COLS, LINES);

    canvas.loadFile(filePath);

    void (Canvas::*defCB)(void) = &Canvas::render;
    void (Canvas::*moveCB)(Direction dir) = &Canvas::moveCursor;
    void (Canvas::*enterInsert)(void) = &Canvas::enterInsertMode;
    void (Canvas::*exitModes)(void) = &Canvas::exitModes;

    InputHandler input(&canvas, defCB);

    input.addKeyCallback(KEY_UP, moveCB, UP, true);
    input.addKeyCallback(KEY_DOWN, moveCB, DOWN, true);
    input.addKeyCallback(KEY_LEFT, moveCB, LEFT, true);
    input.addKeyCallback(KEY_RIGHT, moveCB, RIGHT, true);
    input.addKeyCallback(105, enterInsert);
    input.addKeyCallback(27, exitModes, true);

    input.listen();

    endwin();

    return 0;
}
