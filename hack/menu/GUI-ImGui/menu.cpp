#include "menu.hpp"

#include "tabrender.hpp"
#include "sections/combat/aim.hpp"
#include "sections/visuals/visuals.hpp"
#include "sections/editors/editors.hpp"
#include "sections/misc/misc.hpp"
#include "sections/settings/settings.hpp"

#include <config/vars.hpp>
#include <config/config.hpp>
#include <SDK/InputSystem.hpp>
#include <SDK/interfaces/interfaces.hpp>

#include <cheats/hooks/wndproc.hpp>
#include <cheats/hooks/present.hpp>

namespace
{
	struct MenuDraw : hooks::Present
	{
		MenuDraw()
		{
			this->registerRun(ImGuiMenu::draw);
		}
	} menuDraw;

	struct MenuKeys : hooks::wndProcSys
	{
		MenuKeys()
		{
			this->registerRun(ImGuiMenu::updateKeys);
		}
	} menuKeys;
}

namespace ImGuiMenu
{
	void updateKeys();

	std::string iniFile{ };
	static constexpr auto menuTitle{ "csgo legit" };
}

void ImGuiMenu::updateKeys()
{
	if (vars::keys->menu.isPressed())
	{
		active = !active;

		if (!active)
			memory::interfaces::iSystem->resetState();
	}
}

void ImGuiMenu::setStyles()
{
	ImGuiIO& io = ImGui::GetIO();
	ImGuiStyle& style = ImGui::GetStyle();

	style = vars::styling->imStyle;

	iniFile = std::filesystem::path{ api::getHackPath() / "window.ini" }.string();

	io.IniFilename = iniFile.c_str();
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

void ImGuiMenu::draw()
{
	if (!active)
		return;

	if (ImGui::Begin(menuTitle, &active, ImGuiWindowFlags_NoCollapse))
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