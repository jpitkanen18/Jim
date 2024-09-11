#pragma once
#include <map>
#include <fstream>
#include <sstream>
#include <functional>
#include <ncurses.h>
#include "canvas.h"

class InputHandler {
	std::map<int, void (Canvas::*)(Direction dir)> KeyCallbacks;
	std::map<int, void (Canvas::*)(void)> KeyCallbacksVoid;
	std::map<int, Direction> KeyArgs;
	std::vector<int> InsertIgnoreKeys;
	void (Canvas::*DefaultKeyCallback)(void);
	int Exit;
	Canvas *CanvasPtr;
	public:
		InputHandler(Canvas *canvasPtr , void (Canvas::*defaultKeyCallback)(void)){
			Exit = 0;
			CanvasPtr = canvasPtr;
			DefaultKeyCallback = defaultKeyCallback;
		};

		void addKeyCallback(int key, void (Canvas::*callback)(Direction dir), Direction dir, bool ignorekey = false){
			KeyCallbacks[key] = callback;
			KeyArgs[key] = dir;
			if(ignorekey){
				InsertIgnoreKeys.push_back(key);
			}
		}

		void addKeyCallback(int key, void (Canvas::*callback)(void), bool ignorekey = false){
			KeyCallbacksVoid[key] = callback;
			if(ignorekey){
				InsertIgnoreKeys.push_back(key);
			}
		}

		void listen() {
			while(Exit != 1){
				std::invoke(DefaultKeyCallback, *CanvasPtr);
				int key = getch();
				clear();
				bool ignoreKey = std::find(InsertIgnoreKeys.begin(), InsertIgnoreKeys.end(), key) != InsertIgnoreKeys.end();
				if(CanvasPtr->getInsertMode() == true && ignoreKey == false){
					CanvasPtr->enterCharacter(key);
				} else {
					if(KeyCallbacks.find(key) != KeyCallbacks.end()) {
						void (Canvas::*callbackForKey)(Direction dir) = KeyCallbacks[key];
						if(KeyArgs.find(key) != KeyArgs.end()){
							std::invoke(callbackForKey, *CanvasPtr, KeyArgs[key]);
						}
					}  else if(KeyCallbacksVoid.find(key) != KeyCallbacksVoid.end()) {
						void (Canvas::*callbackForKey)(void) = KeyCallbacksVoid[key];
						std::invoke(callbackForKey, *CanvasPtr);
					}
				}
				std::invoke(DefaultKeyCallback, *CanvasPtr);
			}
		}
};