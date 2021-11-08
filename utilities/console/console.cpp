#include "console.hpp"
#include <fstream>
#include <sstream>
#include "../utilities.hpp"

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

void console::log(const char* fmt, ...)
{
	if (!fmt)
		return;

	if (strlen(fmt) < 2)
		return;

	char buf[256]{};

	va_list args;
	va_start(args, fmt);

	vsnprintf(buf, IM_ARRAYSIZE(buf), fmt, args);

	buf[IM_ARRAYSIZE(buf) - 1] = 0;
	va_end(args);
	std::ofstream log(__PATH + XOR("\\hack.log"), std::ofstream::out | std::ofstream::app);
	std::stringstream ss;
	ss << "[" << utilities::getTime().c_str() << "] " << buf;

	log << ss.str();
	log.close();
#ifdef _DEBUG
	printf("%s", ss.str().c_str());
#endif
	ss.clear();
	return;	
}