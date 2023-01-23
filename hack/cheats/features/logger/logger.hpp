#pragma once

#include <cheats/classes/renderableToPresent.hpp>
#include <render/Color.hpp>

#include <vector>
#include <string>

struct ImFont;
class IGameEvent;

// TODO: run in present
class Logger : protected RenderablePresentType
{
public:
	constexpr Logger() :
		RenderablePresentType{}
	{}

	struct Log_t
	{
		std::string m_text;
		Color m_color;
		float m_timeLog;
		ImFont* m_font;
	};

	void add(const Log_t& log);
protected:
	virtual void init() override;
	virtual void draw() override;
private:
	void handleHits(IGameEvent* event);
	std::vector<Log_t> m_logs;
};

GLOBAL_FEATURE(Logger);