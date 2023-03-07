#pragma once

#include "consoleUtils.hpp"

#include <menu/GUI-ImGui/imguiaddons.hpp>
#include <cheats/classes/renderableToPresent.hpp>
#include <cheats/classes/wndProcKeyHandler.hpp>

#include <vector>
#include <string>

class LogDrawer : protected RenderablePresentType, protected WndProcKeyHandler
{
public:
	LogDrawer()
		: RenderablePresentType{}
	{}
protected:
	virtual void draw() override;
	virtual void updateKeys() override;
public:
	// from demo, slight edit, usage same as normal console.log
	struct ExampleAppLog
	{
		template<typename... Args_t>
		void add(TypeLogs type, const std::string_view fmt, Args_t&&... args);
		void draw(const char* title, bool* p_open = NULL);
	private:
		bool wrap = false;
		ImGuiTextFilter filter;
		std::vector<std::pair<std::string, TypeLogs>> buffer;
	} log;
private:
	bool m_opened = false;
};

GLOBAL_FEATURE(LogDrawer);

template<typename... Args_t>
void LogDrawer::ExampleAppLog::add(TypeLogs logs, const std::string_view fmt, Args_t&&... args)
{
	std::string text{ };
	if constexpr (sizeof...(args) > 0)
		text += std::vformat(fmt, std::make_format_args(args...));
	else
		text += fmt;

	this->buffer.emplace_back(std::make_pair(text, logs));
}