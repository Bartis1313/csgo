#include "menu.hpp"

#include "tabrender.hpp"
#include "sections/combat/aim.hpp"
#include "sections/visuals/visuals.hpp"
#include "sections/editors/editors.hpp"
#include "sections/misc/misc.hpp"
#include "sections/settings/settings.hpp"

#include <config/vars.hpp>
#include <config/config.hpp>

void ImGuiMenu::updateKeys()
{
	if (vars::keys->menu.isPressed())
		m_active = !m_active;
}

void ImGuiMenu::setStyles()
{
	ImGuiIO& io = ImGui::GetIO();
	ImGuiStyle& style = ImGui::GetStyle();

	style = vars::styling->imStyle;

	m_iniFile = std::filesystem::path{ config.getHackPath() / "window.ini" }.string();

	io.IniFilename = m_iniFile.c_str();
	io.LogFilename = nullptr;
}

static std::array allTabs =
{
	TabRender{ "Aimbot", &tabs::aim::draw },
	TabRender{ "Visuals", &tabs::visuals::draw },
	TabRender{ "Misc", &tabs::misc::draw },
	TabRender{ "Settings", &tabs::settings::draw },
	TabRender{ "Editors", &tabs::editors::draw },
};

void ImGuiMenu::renderAll()
{
	if (ImGui::Begin(m_menuTitle, &m_active, ImGuiWindowFlags_NoCollapse))
	{
		ImGuiStyle& style = ImGui::GetStyle();
		ImVec2 backupPadding = style.FramePadding;
		const float width = ImGui::GetContentRegionAvail().x;

		// remove tab underline
		ImGui::PushStyleColor(ImGuiCol_TabActive, Color::U32(Colors::Blank));
		ImGui::PushStyleColor(ImGuiCol_TabUnfocusedActive, Color::U32(Colors::Blank));

		if (ImGui::BeginTabBar("tabbar", ImGuiTabBarFlags_Reorderable))
		{
			style.FramePadding = { width / allTabs.size(), backupPadding.y }; // still this is clamped by imgui in tabs
			ImGui::PopStyleColor(2);
			for (const auto& el : allTabs)
			{
				if (ImGui::BeginTabItem(el.m_name))
				{
					style.FramePadding = backupPadding;
					if (el.funcExist())
						el.m_func();

					ImGui::EndTabItem();
				}
			}
			ImGui::EndTabBar();
		}
	}
	ImGui::End();
}


void ImGuiMenu::draw()
{
	if (m_active)
		renderAll();
}