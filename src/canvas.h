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
	std::vector<int> DataMallocced;
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
					DataMallocced.push_back(line.length() + 1);
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

		std::string removeAt(char* str, int idx) {
			std::string temp(str);			temp.erase(idx, 1);
			return temp;
		}

		void mallocNewLine(int yPos, int size = CHAR_MAX) {
			DataBuffer[yPos] = (char*) malloc(size);
			DataMallocced.push_back(size);
		}

		void enterCharacter(int key) {
			int yPos = CursorLocation.y + OffsetY;
			int xPos = CursorLocation.x + OffsetX;
			if (key == 127) {
				if(xPos == 0 || yPos >= DataMallocced.size()){
					return;
				}
				strcpy(DataBuffer[yPos], removeAt(DataBuffer[yPos], xPos).data());
				CursorLocation.x--;
			} else if(key >= 32 && key <= 126){
				if(yPos >= DataMallocced.size()){
					mallocNewLine(yPos);
					DataBuffer[yPos][xPos] = char(key);
				} else {
					std::string temp(DataBuffer[yPos]);
					temp.insert(xPos, 1, char(key));
					if(strlen(DataBuffer[yPos]) + temp.size() > DataMallocced[yPos]){
						DataBuffer[yPos] = (char*) realloc(NULL, strlen(DataBuffer[yPos]) * 2);
						DataMallocced[yPos] = strlen(DataBuffer[yPos]) * 2;
					}
					strcpy(DataBuffer[yPos], temp.data());
				}
				CursorLocation.x++;
			} else if (key == 10) {
				NumLines++;
				NumLines++;
				moveCursor(DOWN);
				int newYPos = CursorLocation.y + OffsetY;
				if(newYPos >= DataMallocced.size()){
					mallocNewLine(newYPos);
					CursorLocation.x = 0;
				} else if (DataMallocced[newYPos] <= xPos) {
					if(xPos == 0){
						xPos = 4;
					}
					DataBuffer[newYPos] = (char*) realloc(NULL, xPos * 2);
					DataMallocced[newYPos] = xPos * 2;
				} else if (CursorLocation.x < strlen(DataBuffer[yPos]) - 1) {
					std::string temp(DataBuffer[yPos]);
					DataBuffer = (char**) realloc(DataBuffer, sizeof *DataBuffer * NumLines + 1);
					char** tempBuffer = (char**) malloc(sizeof *tempBuffer * NumLines + 1);
					for(int i = yPos; i < NumLines - 1; i++) {
						tempBuffer[i] = (char*) malloc(strlen(DataBuffer[i]) +1 );
						strcpy(tempBuffer[i], DataBuffer[i]);
					}
					for(int i = yPos; i > NumLines - 1; i++){
						strcpy(DataBuffer[i+1], tempBuffer[i]);
						free(tempBuffer[i]);
					}
					free(tempBuffer);
					DataBuffer[newYPos] = (char*) realloc(NULL, temp.substr(xPos, temp.size() - 1).size() * 2);
					strcpy(DataBuffer[yPos], temp.substr(0, xPos).data());
					strcpy(DataBuffer[newYPos], temp.substr(xPos, temp.size() - 1).data());
					DataMallocced.insert(DataMallocced.begin() + newYPos, CHAR_MAX);
					CursorLocation.x = 0;
				}
			}
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
					if(strlen(DataBuffer[CursorLocation.y + OffsetY]) <= 0){
						CursorLocation.x = 0;
					}
					break;
				case DOWN:
					CursorLocation.y++;
					if(CursorLocation.y > Rows) {
						CursorLocation.y = Rows;
						OffsetY++;
					}
					if(CursorLocation.y + OffsetY <= NumLines - 2){
						if(strlen(DataBuffer[CursorLocation.y + OffsetY]) <= 0){
							CursorLocation.x = 0;
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