#include <iostream>
#define APP_NAME "Jim - Nerokas Text Editor"
#define VERSION "1.0.0"
#define AUTHOR "Jesse Pitkänen © 2024"

void println(std::string printable){
	std::cout << printable << std::endl;
}

void printMetadata(){
	println(APP_NAME);
	println(AUTHOR);
	printf("Version: %s\n", VERSION);
	println("");
}

void printArgs(int argc, char** argv){
	std::cout << "argc: " << argc << std::endl;
	for(int i = 1; i < argc; i++){
		std::cout << "argv[" << i << "]: " << argv[i] << std::endl;
	}
	println("");
}
