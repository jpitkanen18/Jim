#pragma once
#include <vector>
#include <fstream>
#include <sstream>
#include <ncurses.h>

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
	public:
		int Columns;
		int Rows;
		int NumLines;
		int OffsetX;
		int OffsetY;
		vec2 CursorLocation;

		Canvas(int col, int row, vec2 savedCursorLoc = vec2()){
			Columns = col;
			Rows = row;
			CursorLocation = savedCursorLoc;
			CanvasBuffer = (char**) malloc(sizeof *CanvasBuffer * Rows); 
			*CanvasBuffer = (char*) malloc(sizeof **CanvasBuffer * Columns);
			OffsetX = 0;
			OffsetY = 5;
			for(int i = 0; i < row; i++){
				std::vector<char> vec(col);
				CanvasBuffer[i] = reinterpret_cast<char*>(vec.data());
			}
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
					strcpy(DataBuffer[i], line.c_str());
					i++;
				}
				inFile.close();
    		}
		}

		void render() {
			clear();
			for(int i = OffsetX; i < Rows; i++) {
				CanvasBuffer[i] = DataBuffer[i];
			}
			for(int i = 0; i < Rows; i++){
				println(CanvasBuffer[i]);
			}
			move(CursorLocation.y, CursorLocation.x);
			refresh();
		}

		void moveCursor(Direction dir){
			switch(dir){
				case UP:
					--CursorLocation.y;
					if(CursorLocation.y < 0)
						CursorLocation.y = 0;
					break;
				case DOWN:
					CursorLocation.y++;
					if(CursorLocation.y > Rows)
						CursorLocation.y = Rows - 1;
					break;
				case LEFT:
					--CursorLocation.x;
					if(CursorLocation.x < 0)
						CursorLocation.x = 0;
					break;
				case RIGHT:
					CursorLocation.x++;
					if(CursorLocation.x > Columns)
						CursorLocation.x = Columns - 1;
					break;
			}
		}

};