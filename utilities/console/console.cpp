#include "console.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <format>
#include <map>
#include "../utilities.hpp"

enum consoleColors
{
	CONSOLE_WHITE = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE, // white on top applies here too, you did not understand this
	CONSOLE_BLACK = 0,
	CONSOLE_DARKBLUE = FOREGROUND_BLUE,
	CONSOLE_DARKGREEN = FOREGROUND_GREEN,
	CONSOLE_DARKCYAN = FOREGROUND_GREEN | FOREGROUND_BLUE,
	CONSOLE_DARKRED = FOREGROUND_RED,
	CONSOLE_DARKMAGENTA = FOREGROUND_RED | FOREGROUND_BLUE,
	CONSOLE_DARKYELLOW = FOREGROUND_RED | FOREGROUND_GREEN,
	CONSOLE_DARKGRAY = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE,
	CONSOLE_GRAY = FOREGROUND_INTENSITY,
	CONSOLE_BLUE = FOREGROUND_INTENSITY | FOREGROUND_BLUE,
	CONSOLE_GREEN = FOREGROUND_INTENSITY | FOREGROUND_GREEN,
	CONSOLE_CYAN = FOREGROUND_INTENSITY | FOREGROUND_GREEN | FOREGROUND_BLUE,
	CONSOLE_RED = FOREGROUND_INTENSITY | FOREGROUND_RED,
	CONSOLE_MAGENTA = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_BLUE,
	CONSOLE_YELLOW = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN,
};

void console::setColor(WORD color)
{
#ifdef _DEBUG
	LF(SetConsoleTextAttribute)(LF(GetStdHandle)(STD_OUTPUT_HANDLE), color);
#endif
	return;
}

void console::reset()
{
#ifdef _DEBUG
	LF(SetConsoleTextAttribute)(LF(GetStdHandle)(STD_OUTPUT_HANDLE), CONSOLE_WHITE);
#endif
	return;
}

bool console::init(const char* title)
{
#ifdef _DEBUG
	static bool bOnce = false;
	if (bOnce)
		return false;

	LF(AllocConsole)();

	freopen_s((_iobuf**)__acrt_iob_func(0), "CONIN$", "r", __acrt_iob_func(0));
	freopen_s((_iobuf**)__acrt_iob_func(0), "CONOUT$", "w", __acrt_iob_func(1));
	freopen_s((_iobuf**)__acrt_iob_func(0), "CONOUT$", "w", __acrt_iob_func(2));

	LF(SetConsoleTitleA)(title);

	bOnce = true;
#endif
	return true;
}

void console::shutdown() 
{
#ifdef _DEBUG
	fclose((_iobuf*)__acrt_iob_func(0));
	fclose((_iobuf*)__acrt_iob_func(1));
	fclose((_iobuf*)__acrt_iob_func(2));
	LF(FreeConsole)();
#endif
	return;
}

static std::map<short, WORD> colors =
{
	{LOG_NO, CONSOLE_WHITE},
	{LOG_INFO, CONSOLE_GREEN},
	{LOG_WELCOME, CONSOLE_CYAN},
	{LOG_ERR, CONSOLE_RED}
};

// I have no idea how to name variables sometimes
static std::map<short, const char*> consoleStrings =
{
	{LOG_NO, ""},
	{LOG_INFO, "[info] "},
	{LOG_WELCOME, "[welcome] "},
	{LOG_ERR, "[err] "}
};

void console::log(const short type, const std::string& str)
{
	if (str.empty())
		return;
	
	std::ofstream log(__PATH + XOR("\\hack.log"), std::ofstream::out | std::ofstream::app);
	std::stringstream ss;

#ifdef _DEBUG
	console::setColor(colors[type]);
	std::cout << consoleStrings[type];
	console::reset();
#endif	

	ss << "[" << utilities::getTime() << "] " << str;

	log << consoleStrings[type] << ss.str();
	log.close();
#ifdef _DEBUG
	std::cout << ss.str();
#endif
	ss.clear();
	return;	
}