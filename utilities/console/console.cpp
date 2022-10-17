#include "console.hpp"

#include <SDK/Color.hpp>

#include <Windows.h>

bool Console::init(const std::string& title, const std::string& logName)
{
	colorsForConsole =
	{
		{ TypeLogs::LOG_NO, ColorsConsole::CONSOLE_WHITE },
		{ TypeLogs::LOG_INFO, ColorsConsole::CONSOLE_CYAN },
		{ TypeLogs::LOG_ERR, ColorsConsole::CONSOLE_DARKRED },
		{ TypeLogs::LOG_WARN, ColorsConsole::CONSOLE_YELLOW }
	};

	consoleStrings = 
	{
		{ TypeLogs::LOG_NO, ""},
		{ TypeLogs::LOG_INFO, "[info]"},
		{ TypeLogs::LOG_ERR, "[err] "},
		{ TypeLogs::LOG_WARN, "[warn]"},
	};

	colorsForView = 
	{
		{ TypeLogs::LOG_NO, Colors::White },
		{ TypeLogs::LOG_INFO, Colors::Green },
		{ TypeLogs::LOG_ERR, Colors::Red },
		{ TypeLogs::LOG_WARN, Colors::Yellow }
	};

	signs = 
	{
		{ TypeLogs::LOG_NO, ' '},
		{ TypeLogs::LOG_INFO, '+' },
		{ TypeLogs::LOG_ERR, '!' },
		{ TypeLogs::LOG_WARN, '*' }
	};

	using typeVeh = std::string;

	std::map<std::string, std::map<typeVeh, size_t>> mappedLimits;

	auto KILLS = mappedLimits["Siege"]["Tank"];

	if (!logName.empty())
		m_logName = logName;

#ifdef _DEBUG
	AllocConsole();
	freopen_s(reinterpret_cast<FILE**>(stdout), "CONOUT$", "w", stdout);
	SetConsoleTitleA(title.c_str());

	return true;
#endif
	return false;
}

void Console::setColor(ColorsConsole color)
{
#ifdef _DEBUG
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), E2T(color));
#endif
	return;
}

void Console::reset()
{
#ifdef _DEBUG
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), E2T(ColorsConsole::CONSOLE_WHITE));
#endif
	return;
}

void Console::shutdown() 
{
#ifdef _DEBUG
	fclose(stdout);
	FreeConsole();
#endif
	return;
}

void Console::drawLog()
{
	if(m_activeLog)
		m_log.Draw(XOR("Logging Console"), &m_activeLog);
}