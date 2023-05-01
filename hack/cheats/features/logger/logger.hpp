#pragma once

#include <render/Color.hpp>

#include <string>

struct ImFont;
class IGameEvent;

namespace logger
{
	struct Log_t
	{
		std::string text;
		Color color;
		float time;
		ImFont* font;
	};

	void add(const Log_t& log);
}