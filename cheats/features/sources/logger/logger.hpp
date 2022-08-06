#pragma once

#include "../../classes/renderableToPresent.hpp"

#include <vector>
#include <string>

#include "../../../SDK/Color.hpp"

struct ImFont;

class Logger : public RenderablePresentType
{
public:
	Logger()
	{
		m_hacks.push_back(this);
		m_hacksRun.push_back(this);
	}

	struct Log_t
	{
		Log_t() = default;

		Log_t(ImFont* font, const std::string& text, const Color& color, float expire)
			: m_font{ font }, m_text { text }, m_color{ color }, m_expire{ expire }
		{}

		std::string m_text;
		Color m_color;
		float m_expire;
		float m_y;
		ImFont* m_font;
	};

	void add(const Log_t& log);
	virtual void draw();
	virtual void init();
private:
	std::vector<Log_t> m_logs = {};
};

[[maybe_unused]] inline auto g_Logger = Logger{};