#pragma once

#include <classes/renderableToSurface.hpp>
#include <SDK/Color.hpp>

#include <vector>
#include <string>

struct ImFont;

// TODO: run in present
class Logger : public RenderableSurfaceType
{
public:
	constexpr Logger() :
		RenderableSurfaceType{}
	{}

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
	std::vector<Log_t> m_logs;
};

[[maybe_unused]] inline auto g_Logger = Logger{};