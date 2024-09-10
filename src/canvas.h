#pragma once
#include <vector>
#include <fstream>
#include <sstream>
#include <ncurses.h>

#define INSERT_TEXT "INSERT"

struct vec2 {
	vec2(): x(0),y(0){ }
	int x, y;
};

enum Direction {
	UP,
	DOWN,
	LEFT,
	RIGHT
};

class Canvas {
	char** CanvasBuffer;
	char** DataBuffer;
	bool insertMode = false;
	public:
		int Columns;
		int Rows;
		int NumLines = 0;
		int OffsetX;
		int OffsetY;
		vec2 CursorLocation;

		Canvas(int col, int row, vec2 savedCursorLoc = vec2()){
			Columns = col;
			Rows = row;
			CursorLocation = savedCursorLoc;
			CanvasBuffer = (char**) malloc(sizeof *CanvasBuffer * Rows);
			OffsetX = 0;
			OffsetY = 0;
			for(int i = 0; i < Rows; i++){
				CanvasBuffer[i] = (char*) malloc(sizeof(char) * Columns - 1);
			}
			clearCanvasBuffer();
		}

		void loadFile(std::string filePath){
			std::string line;
			std::ifstream inFile(filePath);
			
			if(inFile.is_open()) {
				while(!inFile.eof()) { 
					std::getline(inFile, line);
					NumLines++;
				}
    		}
			
			DataBuffer = (char**) malloc(sizeof *DataBuffer * NumLines); 
			if(inFile.is_open()) {
				int i = 0;
				inFile.clear();
				inFile.seekg(0, std::ios::beg);
				while(!inFile.eof()) { 
					std::getline(inFile, line);
					DataBuffer[i] = (char*)malloc(line.length() + 1);
					strcpy(DataBuffer[i], line.data());
					i++;
				}
				inFile.close();
    		}
			if(Rows > NumLines) {
				
			}
		}

		void clearCanvasBuffer() {
			for(int i = 0; i < Rows; i++){
				for(int j = 0; j < Columns - 1; j++) {
					CanvasBuffer[i][j] = '\0';
				}
				CanvasBuffer[i][0] = '~';
			}
		}

		void render() {
			clear();
			clearCanvasBuffer();
			for(int i = 0; i < Rows; i++) {
				if(i + OffsetY > NumLines - 1)
					break;
				strncpy(CanvasBuffer[i], DataBuffer[i + OffsetY], Columns - 2);
			}
			int reduce = 1;
			if(insertMode) {
				reduce = 2;
			}
			for(int i = 0; i < Rows - reduce; i++){
				println(CanvasBuffer[i]);
			}
			if(insertMode){
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

		void removeAt(char* str, int idx) {
			size_t len = strlen(str);
			memmove(str + idx, str + idx + 1, len - idx);
		}

		void enterCharacter(int key) {
			if(key >= 32 && key <= 126){
				DataBuffer[CursorLocation.y][CursorLocation.x] = char(key);
			} else if (key == KEY_BACKSPACE) {
				removeAt(DataBuffer[CursorLocation.y], CursorLocation.x);
			}
			CursorLocation.x++;
		}

		void moveCursor(Direction dir){
			switch(dir){
				case UP:
					--CursorLocation.y;
					if(CursorLocation.y < 0) {
						CursorLocation.y = 0;
						if(OffsetY > 0) {
							OffsetY--;
						}
					}
					break;
				case DOWN:
					CursorLocation.y++;
					if(CursorLocation.y > Rows) {
						CursorLocation.y = Rows;
						if(OffsetY + Rows < NumLines){
							OffsetY++;
						}
					}
					break;
				case LEFT:
					--CursorLocation.x;
					if(CursorLocation.x < 0) {
						CursorLocation.x = 0;
						if(OffsetX > 0) {
							OffsetX--;
						} 
					}
					break;
				case RIGHT:
					CursorLocation.x++;
					if(CursorLocation.x > Columns) {
						CursorLocation.x = Columns - 1;
						if(OffsetX < strlen(DataBuffer[CursorLocation.y])) {
							OffsetX++;
						}
					}
					break;
			}
		}

};