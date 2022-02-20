#include "menu.hpp"
#include "../../../dependencies/ImGui/imgui.h"
#include "../../../dependencies/ImGui/imgui_impl_win32.h"
#include "../../../dependencies/ImGui/imgui_impl_dx9.h"
#include "../../../utilities/utilities.hpp"
#include "../../../SDK/Color.hpp"

void ImGuiMenu::init()
{
	// init styles, config flags etc
}

void ImGuiMenu::shutdown()
{
	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void ImGuiMenu::example()
{
	if (isMenuActive)
	{
		ImGui::Begin(XOR("csgo legit"), &isMenuActive);
		{
			ImGui::End();
		}
	}
}