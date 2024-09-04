#include <fstream>
#include <sstream>
#include <string>

#include <ncurses.h>
#include <sys/ioctl.h>
#include <unistd.h> 

#include "utils.h"
#include "canvas.h"
#include "input.h"

static bool debug = true;

bool test(char* filePath){
	std::ifstream test(filePath); 
	if (!test)
	{
		if(debug){
			println("The file doesn't exist");
		}
		return false;
	} else {
		if(debug){
			print("File found at: ");
			print(filePath);
			println(" :)");
		}
		return true;
	}
}

std::stringstream fileContents(char* filePath){
	std::ifstream inFile;
	inFile.open(filePath);
	std::stringstream strStream;
    strStream << inFile.rdbuf();
	return strStream;
   
}

int main(int argc, char** argv){
	initscr();
	cbreak();
	noecho();
	keypad (stdscr, TRUE);
	scrollok(stdscr, TRUE);
	printMetadata();
	if(debug){
		printArgs(argc, argv);
	}
	struct winsize size;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &size);
	print("ws_col: ");
	println(std::to_string(size.ws_col));
	print("ws_row: ");
	println(std::to_string(size.ws_row));
	if(argc == 1){
		if(debug){
			println("No file provided :(");
		}
		return 1;
	}
	char* filePath = argv[1];
	if(!test(filePath)){
		println("Invalid file path, exiting now :/");
		return 1;
	}

	Canvas canvas(COLS, LINES);

	canvas.loadFile(filePath);

	void (Canvas::*defCB)(void) = &Canvas::render;
	void (Canvas::*moveCB)(Direction dir) = &Canvas::moveCursor;

	InputHandler input(&canvas, defCB);

	input.addKeyCallback(KEY_UP, moveCB, UP);
	input.addKeyCallback(KEY_DOWN, moveCB, DOWN);
	input.addKeyCallback(KEY_LEFT, moveCB, LEFT);
	input.addKeyCallback(KEY_RIGHT, moveCB, RIGHT);

	input.listen();

	endwin();

	return 0;
}
