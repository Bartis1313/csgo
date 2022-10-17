#pragma once

#include "consoleUtils.hpp"

#include <menu/GUI-ImGui/imguiaddons.hpp>
#include <utilities/utilities.hpp>
#include <utilities/tools/tools.hpp>
#include <utilities/tools/wrappers.hpp>
#include <config/config.hpp>

#include <string>
#include <unordered_map>
#include <iostream>
#include <fstream>
#include <mutex>
#include <cassert>

struct SDKColor;

class Console
{
public:
	bool init(const std::string& title, const std::string& logName);
	void shutdown();
	void drawLog();
	constexpr void changeActiveLog() { m_activeLog = !m_activeLog; }
public:
	// logs into console + draw + file
	template<typename... Args_t>
	void log(const char* loc, TypeLogs type, const std::string_view fmt, Args_t&&... args);
private:
	std::unordered_map<TypeLogs, ColorsConsole> colorsForConsole;
	std::unordered_map<TypeLogs, std::string_view> consoleStrings;
	std::unordered_map<TypeLogs, Color> colorsForView;
	std::unordered_map<TypeLogs, char> signs;

	void setColor(ColorsConsole color);
	void reset();

	std::string m_logName;
	ImGui::ExampleAppLog m_log;
	bool m_activeLog = false;
	std::mutex mutex;
};

// sorry for using cout
// https://youtu.be/TxhQQLGafZ4?t=78
// :D

template<typename... Args_t>
void Console::log(const char* loc, TypeLogs type, const std::string_view fmt, Args_t&&... args)
{
	std::scoped_lock lock{ mutex };

	if (m_logName.empty())
		assert("Did you call Console::init ?");

	if (fmt.empty())
		return;

	std::ofstream log{ config.getHackPath() / m_logName, std::ofstream::out | std::ofstream::app };
	std::string buffer{};

#ifdef _DEBUG
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), E2T(colorsForConsole[type]));
	if (type != TypeLogs::LOG_NO)
		std::cout << FORMAT(XOR("{}{} {} "), signs[type], consoleStrings[type], loc);

	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), E2T(ColorsConsole::CONSOLE_WHITE));
#endif

	buffer += FORMAT(XOR("[{}] "), utilities::getTime());

	if constexpr (sizeof...(args) > 0)
		buffer += std::vformat(fmt, std::make_format_args(args...));
	else
		buffer += fmt;

	buffer += '\n';

#ifdef _DEBUG
	std::cout << buffer;
#endif

	log << buffer;
	m_log.AddLog(buffer);

	log.close();
}

inline Console console;

#define LOG_INFO(fmt, ...) console.log(XOR(__FUNCTION__), TypeLogs::LOG_INFO, fmt, __VA_ARGS__)
#define LOG_ERR(fmt, ...) console.log(XOR(__FUNCTION__), TypeLogs::LOG_ERR, fmt, __VA_ARGS__)
#define LOG_WARN(fmt, ...) console.log(XOR(__FUNCTION__), TypeLogs::LOG_WARN, fmt, __VA_ARGS__)
#define LOG_EMPTY(fmt, ...) console.log(XOR(__FUNCTION__), TypeLogs::LOG_NO, fmt, __VA_ARGS__)