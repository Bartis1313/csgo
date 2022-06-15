#pragma once

#include <vector>
#include <string>

#include "../../../SDK/Color.hpp"

class Logger
{
public:
	struct Log_t
	{
		Log_t() = default;

		Log_t(const std::string& text, const Color& color, float expire)
			: m_text{ text }, m_color{ color }, m_expire{ expire }
		{}

		std::string m_text;
		Color m_color;
		float m_expire;
		float m_y;
	};

	void add(const Log_t& log);
	void draw();
private:
	std::vector<Log_t> m_logs = {};
};

inline Logger logger;