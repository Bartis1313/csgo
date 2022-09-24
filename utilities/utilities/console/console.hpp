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
	void log(TypeLogs type, const std::string_view fmt, Args_t&&... args);

	// only puts it inside drawing
	// There is no date, no automatic newline
	template<typename... Args_t>
	void print(const std::string& fmt, Args_t&&... args);
private:
	std::unordered_map<TypeLogs, ColorsConsole> colorsForConsole;
	std::unordered_map<TypeLogs, std::string_view> consoleStrings;
	std::unordered_map<TypeLogs, SDKColor> colorsForView;

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