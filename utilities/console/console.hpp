#pragma once
#include <Windows.h>
#include <string>

enum TypeLogs
{
	// use it when color is not needed
	LOG_NO,
	LOG_INFO,
	LOG_WELCOME,
	LOG_ERR
};

namespace console
{
	bool init(const char* title);
	void shutdown();
	void setColor(WORD color);
	void reset();
	void log(const short type, const std::string& str, const bool useNewLine = true);
}