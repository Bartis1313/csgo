#include "consoleDraw.hpp"
#include "console.hpp"

#include <render/Color.hpp>
#include <utilities/tools/tools.hpp>

#include <cheats/hooks/present.hpp>
#include <cheats/hooks/wndproc.hpp>

namespace
{
	struct LogDraw : hooks::Present
	{
		LogDraw()
		{
			this->registerRun(logDrawer::draw);
		}
	} logDraw;

	struct LogKeys : hooks::wndProcSys
	{
		LogKeys()
		{
			this->registerRun(logDrawer::updateKeys);
		}
	} logKeys;
}

void logDrawer::ExampleAppLog::draw(const char* title, bool* p_open)
{
	if (!ImGui::Begin(title, p_open))
	{
		ImGui::End();
		return;
	}

	filter.Draw("Filter");

	ImGui::SameLine();
	bool clear = ImGui::Button("Clear");
	ImGui::SameLine();
	bool copy = ImGui::Button("Copy");
	ImGui::SameLine();
	if (ImGui::Button("Wrap"))
		wrap = !wrap;

	ImGui::Spacing();

	ImGui::BeginChild("scrolling", ImVec2(0, 0), false, wrap ? ImGuiWindowFlags_AlwaysHorizontalScrollbar : ImGuiWindowFlags_HorizontalScrollbar);

	if (clear)
		buffer.clear();
	if (copy)
		ImGui::LogToClipboard();

	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
	if (filter.IsActive())
	{
		for (const auto [text, color] : buffer)
		{
			if (filter.PassFilter(text.data()))
			{
				if (wrap)
					ImGui::PushTextWrapPos();
				ImGui::PushStyleColor(ImGuiCol_Text, Color::U32(console::detail::colorsForView.at(color)));
				ImGui::TextUnformatted(text.data());
				ImGui::PopStyleColor();
				if (wrap)
					ImGui::PopTextWrapPos();
			}
		}
	}
	else
	{
		ImGuiListClipper clipper;
		clipper.Begin(buffer.size());
		while (clipper.Step())
		{
			for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++)
			{
				const auto [text, color] = buffer.at(i);

				if (wrap)
					ImGui::PushTextWrapPos();
				ImGui::PushStyleColor(ImGuiCol_Text, Color::U32(console::detail::colorsForView.at(color)));
				ImGui::TextUnformatted(text.data());
				ImGui::PopStyleColor();
				if (wrap)
					ImGui::PopTextWrapPos();
			}
		}
		clipper.End();
	}
	ImGui::PopStyleVar();

	if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
		ImGui::SetScrollHereY(1.0f);

	ImGui::EndChild();
	ImGui::End();
}

void logDrawer::draw()
{
	if (opened)
		log.draw("Logging Console", &opened);
}

#include <config/vars.hpp>

void logDrawer::updateKeys()
{
	if (vars::keys->console.isPressed())
		opened = !opened;
}