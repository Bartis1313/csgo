#pragma once

#include <Windows.h>
#include <string>
#include <map>
#include <iostream>
#include <fstream>
#include <format>
#include <cassert>
#include <locale>
#include <mutex>

#include "../../SDK/Color.hpp"
#include "../../config/config.hpp"
#include "../../cheats/menu/GUI-ImGui/imguiaddons.hpp"

enum class TypeLogs : size_t
{
	// use it when color is not needed
	LOG_NO,
	LOG_INFO,
	LOG_ERR,
	LOG_WARN
};

class Console
{
public:
	bool init(const std::string& title, const std::string& logName);
	void shutdown();
	constexpr std::string getLogName() const { return m_logName; }
	void drawLog();
	constexpr void changeActiveLog() { m_activeLog = !m_activeLog; }
private:
#pragma region maps_and_colors
	enum class ColorsConsole : size_t
	{
		CONSOLE_WHITE = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE,
		CONSOLE_BLACK = NULL,
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

	std::map<TypeLogs, ColorsConsole> colorsForConsole =
	{
		{TypeLogs::LOG_NO, ColorsConsole::CONSOLE_WHITE},
		{TypeLogs::LOG_INFO, ColorsConsole::CONSOLE_GREEN},
		{TypeLogs::LOG_ERR, ColorsConsole::CONSOLE_DARKRED},
		{TypeLogs::LOG_WARN, ColorsConsole::CONSOLE_YELLOW}
	};

	std::map<TypeLogs, const char*> consoleStrings =
	{
		{TypeLogs::LOG_NO, ""},
		{TypeLogs::LOG_INFO, "[info] "},
		{TypeLogs::LOG_ERR, "[err] "},
		{TypeLogs::LOG_WARN, "[warn] "},
	};

	std::map<TypeLogs, SDKColor> colorsForView =
	{
		{TypeLogs::LOG_NO, Colors::White },
		{TypeLogs::LOG_INFO, Colors::Green },
		{TypeLogs::LOG_ERR, Colors::Red },
		{TypeLogs::LOG_WARN, Colors::Yellow }
	};

#pragma endregion
	void setColor(ColorsConsole color);
	void reset();

	std::string m_logName;
	ImGui::ExampleAppLog m_log;
	bool m_activeLog = false;
	std::mutex mutex;
public:
	// logs into console + draw + file
	template<typename... Args_t>
	void log(TypeLogs type, const std::string_view fmt, Args_t&&... args);

	// only puts it inside drawing
	// There is no date, no automatic newline
	template<typename... Args_t>
	void print(const std::string& fmt, Args_t&&... args);
};

// sorry for using cout
// https://youtu.be/TxhQQLGafZ4?t=78
// :D

template<typename... Args_t>
void Console::log(TypeLogs type, const std::string_view fmt, Args_t&&... args)
{
	std::scoped_lock lock{ mutex };

	if (m_logName.empty())
		assert("Did you call Console::init ?");

	if (fmt.empty())
		return;

	std::ofstream log{ config.getPathForSave(m_logName), std::ofstream::out | std::ofstream::app };

#ifdef _DEBUG
	setColor(colorsForConsole[type]);
	std::cout << consoleStrings[type];
	reset();
#endif
	std::string buffer;

	buffer += FORMAT(XOR("[{}] "), utilities::getTime());

	if constexpr (sizeof...(args) > 0)
		buffer += std::vformat(std::locale(), fmt, std::make_format_args(args...));
	else
		buffer += fmt;

	buffer += '\n';
#ifdef _DEBUG
	std::cout << buffer;
#endif
	log << consoleStrings[type] << buffer;
	m_log.AddLog(buffer);
	log.close();
}

template<typename... Args_t>
void Console::print(const std::string& fmt, Args_t&&... args)
{
	std::scoped_lock lock{ mutex };
	// we don't need to take of anything because I set it already
	m_log.AddLog(fmt, args...);
}

inline Console console;