#include "console.hpp"

#include <menu/GUI-ImGui/imguiaddons.hpp>
#include <utilities/utilities.hpp>
#include <utilities/tools/tools.hpp>
#include <utilities/tools/wrappers.hpp>
#include <render/Color.hpp>

#include <Windows.h>

void console::setLogger(const std::string_view title, const std::string_view logName)
{
	detail::m_logName = logName;
	assert(title.empty() == false); // not needed in release, will make code ugly if we add #idfefs everywhere
	assert(logName.empty() == false);

	detail::m_fileStream = std::ofstream{ config.getHackPath() / detail::m_logName, std::ofstream::out | std::ofstream::app };

#ifdef _DEBUG
	AllocConsole();
	freopen_s(reinterpret_cast<FILE**>(stdout), "CONOUT$", "w", stdout);
	SetConsoleTitleA(title.data());
#endif
}

void console::detail::setColor(ColorsConsole color)
{
#ifdef _DEBUG
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), E2T(color));
#endif
	return;
}

void console::detail::reset()
{
#ifdef _DEBUG
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), E2T(ColorsConsole::CONSOLE_WHITE));
#endif
	return;
}

void console::shutdown() 
{
#ifdef _DEBUG
	fclose(stdout);
	FreeConsole();
#endif

	detail::m_fileStream.close();
	return;
}

void console::detail::addLogToStream(TypeLogs type, const std::string& buf)
{
#ifdef _DEBUG
	std::cout << buf;
#endif

	m_fileStream << buf;
	g_LogDrawer->log.add(type, buf);
}

bool console::detail::isPossibleToLog(TypeLogs type)
{
#ifndef _DEBUG
	if (type == TypeLogs::LOG_ERR || type == TypeLogs::LOG_INFO)
		return true;

	return false;
#else
	return true;
#endif
}

std::string console::detail::generateTimeLog()
{
	return std::format("[{}] ", utilities::getTime());
}

void console::detail::addSigns(TypeLogs type)
{
#ifdef _DEBUG
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), E2T(colorsForConsole[type]));
	std::cout << std::format("{} {}", signs[type], consoleStrings[type]);
	reset();
#endif
}