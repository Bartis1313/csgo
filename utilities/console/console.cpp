#include "console.hpp"

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

bool Console::init(const std::string& title, const std::string& logName)
{
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