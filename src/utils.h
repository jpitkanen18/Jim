#pragma once
#include <ncurses.h>
#define APP_NAME "Jim - Nerokas Text Editor"
#define VERSION "1.0.0"
#define AUTHOR "Jesse Pitkänen © 2024"

void print(std::string printable)
{
	printw("%s", printable.data());
}

void println(std::string printable)
{
	printw("%s\n", printable.data());
}

void println(wchar_t *printable)
{
	std::string lol((char *)printable);

	println(lol);
}

void println(char *printable)
{
	std::string lol(printable);

	println(lol);
}

void println(const char *printable)
{
	std::string lol(printable);
	println(lol);
}

void println(char printable)
{
	std::string str(1, printable);
	println(str);
}

void println(int printable)
{
	println(std::to_string(printable));
}

void printMetadata()
{
	println(APP_NAME);
	println(AUTHOR);
	printf("Version: %s\n", VERSION);
	println("");
}

void printArgs(int argc, char **argv)
{
	print("Argc: ");
	println(std::to_string(argc));
	for (int i = 1; i < argc; i++)
	{
		print("argv[");
		print(std::to_string(i));
		print("]: ");
		println(argv[i]);
	}
	println("");
}

void clearTerminal()
{
	print("\033c");
}
