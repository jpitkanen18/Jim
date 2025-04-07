#include <ncurses.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include <cstring>
#include <fstream>
#include <sstream>
#include <string>

#include "Jim/Canvas.hpp"
#include "Jim/InputHandler.hpp"
#include "Jim/Utils.hpp"

void setup(int argc, char *argv[]);

int main(int argc, char *argv[]) {
    using namespace Utils;
    using namespace Jim;
    setup(argc, argv);

    if (argc == 1) {
#ifdef DEBUG
        Println("No file provided :(");
#endif
        return 1;
    }

    char *filePath = argv[1];
    if (!Test(filePath)) {
#ifdef DEBUG
        Println("Invalid file path, exiting now :/");
#endif
        return 1;
    }

    Canvas canvas(COLS, LINES);

    canvas.LoadFile(filePath);

    canvas.Render();
    void (Canvas::*defCB)(void) = &Canvas::Render;
    void (Canvas::*moveCB)(Direction dir) = &Canvas::MoveCursor;
    void (Canvas::*enterInsert)(void) = &Canvas::EnterInsertMode;
    void (Canvas::*enterCommandPalette)(void) = &Canvas::EnterCommandPalette;
    void (Canvas::*exitModes)(void) = &Canvas::ExitModes;

    InputHandler<
        Canvas, void (Canvas::*)(Direction dir),
        void (Canvas::*)(void),
        Direction>
        input(&canvas, defCB);

    input.AddKeyCallback(KEY_UP, moveCB, UP, true);
    input.AddKeyCallback(KEY_DOWN, moveCB, DOWN, true);
    input.AddKeyCallback(KEY_LEFT, moveCB, LEFT, true);
    input.AddKeyCallback(KEY_RIGHT, moveCB, RIGHT, true);
    input.AddKeyCallback(105, enterInsert);
    input.AddKeyCallback(27, exitModes, true);
    input.AddKeyCallback(58, enterCommandPalette);

    input.Listen();

    endwin();

    return 0;
}

void setup(int argc, char *argv[]) {
    using namespace Utils;
    setlocale(LC_ALL, "");
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    scrollok(stdscr, TRUE);
    PrintMetadata();
#ifdef DEBUG
    PrintArgs(argc, argv);
#endif
    struct winsize size;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &size);
#ifdef DEBUG
    Print("ws_col: ");
    Println(size.ws_col);
    Print("ws_row: ");
    Println(size.ws_row);
#endif
}
