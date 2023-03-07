#include "consoleDraw.hpp"
#include "console.hpp"

#include <render/Color.hpp>
#include <utilities/tools/tools.hpp>
#include <menu/GUI-ImGui/animations.hpp>

void LogDrawer::ExampleAppLog::draw(const char* title, bool* p_open)
{
	if (!ImGui::Begin(title, p_open))
	{
		ImGui::End();
		return;
	}

	filter.Draw("Filter");

	ImGui::SameLine();
	bool clear = ImGui::Animations::Button("Clear");
	ImGui::SameLine();
	bool copy = ImGui::Animations::Button("Copy");
	ImGui::SameLine();
	if (ImGui::Animations::Button("Wrap"))
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

void LogDrawer::draw()
{
	if (m_opened)
		log.draw("Logging Console", &m_opened);
}

#include <config/vars.hpp>

void LogDrawer::updateKeys()
{
	if (vars::keys->console.isPressed())
		m_opened = !m_opened;
}