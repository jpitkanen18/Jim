#pragma once
#include <map>
#include <fstream>
#include <sstream>
#include <functional>
#include <ncurses.h>
#include "canvas.h"

class InputHandler {
	std::map<int, void (Canvas::*)(Direction dir)> KeyCallbacks;
	std::map<int, Direction> KeyArgs;
	void (Canvas::*DefaultKeyCallback)(void);
	int Exit;
	Canvas *CanvasPtr;
	public:
		InputHandler(Canvas *canvasPtr , void (Canvas::*defaultKeyCallback)(void)){
			Exit = 0;
			CanvasPtr = canvasPtr;
			DefaultKeyCallback = defaultKeyCallback;
		};

		void addKeyCallback(int key, void (Canvas::*callback)(Direction dir), Direction dir){
			KeyCallbacks[key] = callback;
			KeyArgs[key] = dir;
		}

		void listen() {
			while(Exit != 1){
				std::invoke(DefaultKeyCallback, *CanvasPtr);
				int key = getch();
				clear();
				println(std::to_string(key));
				if(KeyCallbacks.find(key) != KeyCallbacks.end()) {
					void (Canvas::*callbackForKey)(Direction dir) = KeyCallbacks[key];
					if(KeyArgs.find(key) != KeyArgs.end()){
						std::invoke(callbackForKey, *CanvasPtr, KeyArgs[key]);
					}
				}
				std::invoke(DefaultKeyCallback, *CanvasPtr);
			}
		}
};