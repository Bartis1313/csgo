#include "editors.hpp"

#include "../common.hpp"
#include "../../tabrender.hpp"
#include "chamsEditor.hpp"
#include "styleEditor.hpp"

#include <cheats/features/visuals/chams/editor.hpp>

std::array allTabs
{
	TabRender{ "Chams", &chamsEditor::draw },
	TabRender{ "Menu style", &styleEditor::draw }
};

void tabs::editors::draw()
{
	ImGuiStyle& style = ImGui::GetStyle();
	ImVec2 backupPadding = style.FramePadding;
	const float width = ImGui::GetContentRegionAvail().x;

	// remove tab underline
	ImGui::PushStyleColor(ImGuiCol_TabActive, Color::U32(Colors::Blank));
	ImGui::PushStyleColor(ImGuiCol_TabUnfocusedActive, Color::U32(Colors::Blank));

	if (ImGui::BeginTabBar("tabbareditor"))
	{
		style.FramePadding = { width / allTabs.size(), backupPadding.y }; // still this is clamped by imgui in tabs
		ImGui::PopStyleColor(2);

		for (size_t i = 0; const auto & tab : allTabs)
		{
			if (ImGui::BeginTabItem(tab.m_name))
			{
				style.FramePadding = backupPadding;

				if (tab.funcExist())
					tab.m_func();

				ImGui::EndTabItem();
			}
		}
		ImGui::EndTabBar();
	}
}
