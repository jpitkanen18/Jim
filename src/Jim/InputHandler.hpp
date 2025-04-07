#pragma once
#include <ncurses.h>

#include <algorithm>
#include <fstream>
#include <functional>
#include <map>
#include <sstream>

#include "Canvas.hpp"

namespace Jim {
/*
    NOTE: This is intentionally janky :D
    The `ClassName` class needs to have a `GetInsertMode`
    method as well as an `EnterCharacter(key)` method
*/
template <typename ClassName, typename FunctionType, typename VoidFunctionType, typename FunctionArgsType>
class InputHandler {
    std::map<int, FunctionType> KeyCallbacks;
    std::map<int, VoidFunctionType> KeyCallbacksVoid;
    std::map<int, FunctionArgsType> KeyArgs;
    std::vector<int> InsertIgnoreKeys;
    void (ClassName::*DefaultKeyCallback)(void);
    int Exit;
    ClassName *ClassPtr;

   public:
    InputHandler(ClassName *classPtr, void (ClassName::*callback)(void)) {
        Exit = 0;
        ClassPtr = classPtr;
        DefaultKeyCallback = callback;
    };

    void AddKeyCallback(int key, FunctionType callback, FunctionArgsType dir, bool ignorekey = false) {
        KeyCallbacks[key] = callback;
        KeyArgs[key] = dir;
        if (ignorekey) {
            InsertIgnoreKeys.push_back(key);
        }
    }

    void AddKeyCallback(int key, void (ClassName::*callback)(void), bool ignorekey = false) {
        KeyCallbacksVoid[key] = callback;
        if (ignorekey) {
            InsertIgnoreKeys.push_back(key);
        }
    }

    void Listen() {
        while (Exit != 1) {
            (ClassPtr->*DefaultKeyCallback)();
            int key = getch();
            // printw("\033c");
            // refresh();
            // printw("%d", key);
            // refresh();
            // continue;
            bool ignoreKey = std::find(InsertIgnoreKeys.begin(), InsertIgnoreKeys.end(), key) != InsertIgnoreKeys.end();
            if (ClassPtr->GetInsertMode() == true && ignoreKey == false) {
                ClassPtr->EnterCharacter(key);
            } else if (ClassPtr->GetCommandPalette() == true && ignoreKey == false) {
                ClassPtr->PushToCommandPalette(key);
            } else {
                if (KeyCallbacks.find(key) != KeyCallbacks.end()) {
                    void (ClassName::*callbackForKey)(FunctionArgsType dir) = KeyCallbacks[key];
                    if (KeyArgs.find(key) != KeyArgs.end()) {
                        (ClassPtr->*callbackForKey)(KeyArgs[key]);
                    }
                } else if (KeyCallbacksVoid.find(key) != KeyCallbacksVoid.end()) {
                    void (ClassName::*callbackForKey)(void) = KeyCallbacksVoid[key];
                    (ClassPtr->*callbackForKey)();
                }
            }
            (ClassPtr->*DefaultKeyCallback)();
        }
    }
};
}  // namespace Jim