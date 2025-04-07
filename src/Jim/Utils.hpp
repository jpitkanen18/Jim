#pragma once
#include <ncurses.h>

#include <iostream>

#define APP_NAME "Jim - Nerokas Text Editor"
#define VERSION "1.0.0"
#define AUTHOR "Jesse Pitkänen © 2024"

#include "Canvas.hpp"

namespace Utils {

void Print(std::string printable) {
    printw("%s", printable.data());
}

void Println(std::string printable, const char *format = "%s\n") {
    printw(format, printable.data());
}

void Println(wchar_t *printable) {
    std::string lol((char *)printable);
    Println(lol);
}

void Println(char *printable) {
    std::string lol(printable);
    Println(lol);
}

void Println(const char *printable) {
    std::string lol(printable);
    Println(lol);
}

void Println(char printable) {
    std::string str(1, printable);
    Println(str);
}

void Println(int printable) {
    Println(std::to_string(printable));
}

void Println(Jim::Line *printable) {
    for (int i = 0; i < printable->length; i++) {
        printw("%s", printable->chars[i].content);
    }
    printw("\n");
}

void Println(Jim::Character *printable) {
    printw(printable->content);
}

void PrintMetadata() {
    Println(APP_NAME);
    Println(AUTHOR);
    Println(VERSION, "Version: %s\n");
    Println("");
    refresh();
}

void PrintArgs(int argc, char **argv) {
    Print("Argc: ");
    Println(std::to_string(argc));
    for (int i = 1; i < argc; i++) {
        Print("argv[");
        Print(std::to_string(i));
        Print("]: ");
        Println(argv[i]);
    }
    Println("");
    refresh();
}

void ClearTerminal() {
    Print("\033c");
    refresh();
}

std::stringstream FileContents(char *filePath) {
    std::ifstream inFile;
    inFile.open(filePath);
    std::stringstream strStream;
    strStream << inFile.rdbuf();
    return strStream;
}

void WriteToDisk(Jim::Line *buffer, int numLines, std::string filePath) {
    std::stringstream strStream;
    std::ofstream output(filePath);
    for (int i = 0; i < numLines; i++) {
        for (int j = 0; j < buffer[i].length; j++) {
            output << buffer[i].chars[j].content;
        }
        if (i != numLines - 1) {
            output << '\n';
        }
    }
};

bool Test(char *filePath) {
    std::ifstream test(filePath);
    if (!test) {
#ifdef DEBUG
        Println("The file doesn't exist");
#endif
        return false;
    } else {
#ifdef DEBUG
        Print("File found at: ");
        Print(filePath);
        Println(" :)");
#endif
        return true;
    }
}
}  // namespace Utils
