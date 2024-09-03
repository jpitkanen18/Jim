#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>

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
	char** canvasBuffer;
	char** dataBuffer;
	public:
		int Columns;
		int Rows;
		int numLines;
		vec2 CursorLocation;

		Canvas(int col, int row, vec2 savedCursorLoc = vec2()){
			Columns = col;
			Rows = row;
			CursorLocation = savedCursorLoc;
			canvasBuffer = (char**) malloc(sizeof *canvasBuffer * Rows); 
			*canvasBuffer = (char*) malloc(sizeof **canvasBuffer * Columns);
			for(int i = 0; i < row; i++){
				std::vector<char> vec(col);
				canvasBuffer[i] = reinterpret_cast<char*>(vec.data());
			}
			for(int i = 0; i < Rows; i++){
				std::cout << canvasBuffer[i] << std::endl;
			}
		}

		void loadFile(std::string filePath){
			std::string line;
			std::ifstream inFile(filePath);
			dataBuffer = (char**) malloc(sizeof *dataBuffer * Rows); 
			*dataBuffer = (char*) malloc(sizeof **dataBuffer * Columns);
			if(inFile.is_open()) {
				while(!inFile.eof()) { 
					std::getline(inFile, line);
					dataBuffer[numLines] =  strcpy((char*)malloc(line.length()+1), line.c_str());
					numLines++;
				}
				inFile.close();
    		}

		}

		void moveCursor(Direction dir){
			switch(dir){
				case UP:
					if(CursorLocation.y <= 0)
						CursorLocation.y = 0;
					CursorLocation.y -= 1;
				case DOWN:
					if(CursorLocation.y >= Rows)
						CursorLocation.y = 0;
					CursorLocation.y += 1;
				case LEFT:
					if(CursorLocation.x <= 0)
						CursorLocation.x = 0;
					CursorLocation.x -= 1;
				case RIGHT:
					if(CursorLocation.x >= Columns)
						CursorLocation.x = 0;
					CursorLocation.x += 1;
			}
		}

};