#pragma once

#include "consoleUtils.hpp"

#include <config/config.hpp>
#include <render/Color.hpp>

#include <string>
#include <unordered_map>
#include <iostream>
#include <fstream>
#include <mutex>
#include <cassert>
#include "consoleDraw.hpp"

struct SDKColor;

namespace console
{
	void setLogger(const std::string_view title, const std::string_view logName);
	void shutdown();
	template<typename... Args_t>
	inline void log(TypeLogs type, const std::string_view fmt, Args_t&&... args);
	template<typename... Args_t>
	inline void error(const std::string_view fmt, Args_t&&... args) { log(TypeLogs::LOG_ERR, fmt, args...); }
	template<typename... Args_t>
	inline void warn(const std::string_view fmt, Args_t&&... args) { log(TypeLogs::LOG_WARN, fmt, args...); }
	template<typename... Args_t>
	inline void info(const std::string_view fmt, Args_t&&... args) { log(TypeLogs::LOG_INFO, fmt, args...); }
	template<typename... Args_t>
	inline void debug(const std::string_view fmt, Args_t&&... args) { log(TypeLogs::LOG_DEBUG, fmt, args...); }

	namespace detail
	{
		void setColor(ColorsConsole color);
		void reset();
		void addSigns(TypeLogs type);
		void addLogToStream(TypeLogs type, const std::string& buf);
		bool isPossibleToLog(TypeLogs type);
		std::string generateTimeLog();

		inline std::ofstream m_fileStream;
		inline std::string m_logName;
		inline std::mutex m_mutex;

		inline std::unordered_map<TypeLogs, ColorsConsole> colorsForConsole
		{
			{ TypeLogs::LOG_ERR, ColorsConsole::CONSOLE_DARKRED },
			{ TypeLogs::LOG_WARN, ColorsConsole::CONSOLE_YELLOW },
			{ TypeLogs::LOG_INFO, ColorsConsole::CONSOLE_WHITE },
			{ TypeLogs::LOG_DEBUG, ColorsConsole::CONSOLE_CYAN },
		};
		inline std::unordered_map<TypeLogs, std::string_view> consoleStrings
		{
			{ TypeLogs::LOG_ERR, "[err]"},
			{ TypeLogs::LOG_WARN, "[warn]"},
			{ TypeLogs::LOG_INFO, "[info]"},
			{ TypeLogs::LOG_DEBUG, "[debug]"},
		};
		inline std::unordered_map<TypeLogs, Color> colorsForView
		{
			{ TypeLogs::LOG_ERR, Colors::Red },
			{ TypeLogs::LOG_WARN, Colors::Yellow },
			{ TypeLogs::LOG_INFO, Colors::White },
			{ TypeLogs::LOG_DEBUG, Colors::Cyan },
		};
		inline std::unordered_map<TypeLogs, char> signs
		{
			{ TypeLogs::LOG_ERR, '!'},
			{ TypeLogs::LOG_WARN, '^'},
			{ TypeLogs::LOG_INFO, '+'},
			{ TypeLogs::LOG_DEBUG, '*'},
		};
	}
};

// sorry for using cout
// https://youtu.be/TxhQQLGafZ4?t=78
// :D

template<typename... Args_t>
inline void console::log(TypeLogs type, const std::string_view fmt, Args_t&&... args)
{
	std::scoped_lock lock{ detail::m_mutex };

	if (!detail::isPossibleToLog(type))
		return;

	if (fmt.empty())
		return;

	detail::addSigns(type);
	std::string buffer = detail::generateTimeLog();
	if constexpr (sizeof...(args) > 0)
		buffer += std::vformat(fmt, std::make_format_args(args...));
	else
		buffer += fmt;

	buffer += '\n';

	detail::addLogToStream(type, buffer);
}