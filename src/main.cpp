#include <iostream>
#include <fstream>
#include <sstream>

#include <sys/ioctl.h>
#include <unistd.h> 
#include "metadata.h"
#include "canvas.h"

static bool debug = true;

bool test(char* filePath){
	std::ifstream test(filePath); 
	if (!test)
	{
		if(debug){
			std::cout << "The file doesn't exist" << std::endl;
		}
		return false;
	} else {
		if(debug){
			std::cout << "File found at: " << filePath << " :)" << std::endl;
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
	printMetadata();
	if(debug){
		printArgs(argc, argv);
	}
	struct winsize size;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &size);
	std::cout << "ws_col: " << size.ws_col << std::endl;
	std::cout << "ws_row: " << size.ws_row << std::endl;
	if(argc == 1){
		if(debug){
			std::cout << "No file provided :(" << std::endl;
		}
		return 1;
	}
	char* filePath = argv[1];
	if(!test(filePath)){
		return 1;
	}

	std::stringstream contents = fileContents(filePath);
	std::cout << contents.str() << "\n";

	Canvas canvas(size.ws_col, size.ws_row);

	canvas.loadFile(filePath);

	return 0;
}