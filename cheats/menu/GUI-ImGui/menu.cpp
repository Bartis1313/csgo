#include "menu.hpp"
#include "../../../dependencies/ImGui/imgui.h"
#include "../../../dependencies/ImGui/imgui_impl_win32.h"
#include "../../../dependencies/ImGui/imgui_impl_dx9.h"
#include "../../../utilities/utilities.hpp"
#include "../../../utilities/renderer/renderer.hpp"
#include "../../../config/vars.hpp"

// see the code of dear imgui lib or demo window to change it.
void ImGuiMenu::init()
{
	ImGui::StyleColorsDark();

	ImGuiStyle& style = ImGui::GetStyle();

	style.ScrollbarSize = 8.0f;
	style.WindowRounding = 8.0f;
	style.Alpha = 0.8f;

	ImVec4* colors = ImGui::GetStyle().Colors;

	// TEXT

	colors[ImGuiCol_Text] = U32(Colors::White);
	colors[ImGuiCol_TextDisabled] = U32(Color(120, 120, 120, 255));

	// WINDOW

	colors[ImGuiCol_WindowBg] = U32(Colors::LightBlue);

	// SCROLL

	style.Colors[ImGuiCol_ScrollbarGrab] = U32(Colors::Yellow);
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = U32(Color(160, 70, 140, 255));
	style.Colors[ImGuiCol_ScrollbarGrabActive] = U32(Color(120, 70, 140, 255));

	// HEADER

	style.Colors[ImGuiCol_Header] = U32(Colors::Yellow);
	style.Colors[ImGuiCol_HeaderHovered] = U32(Color(160, 70, 140, 255));
	style.Colors[ImGuiCol_HeaderActive] = U32(Color(120, 70, 140, 255));

	// add more

	ImGuiIO& io = ImGui::GetIO();

	// NEED static here
	static auto p = config.getPathForSave(XOR("window.ini")).string();

	io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
	io.IniFilename = p.c_str();
	io.LogFilename = nullptr;

	try
	{
		imRender.init(io);
	}
	catch (const std::runtime_error& err)
	{
		LOG(LOG_ERR, err.what());
	}
}

void ImGuiMenu::shutdown()
{
	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	if(ImGui::GetCurrentContext())
		ImGui::DestroyContext();
}

void ImGuiMenu::example()
{
	if (isMenuActive)
	{
		ImGui::ShowDemoWindow(&isMenuActive);
	}
}

void ImGuiMenu::draw()
{
	if (isMenuActive)
	{
		ImGui::Begin(XOR("csgo legit"), &isMenuActive);
		{
			ImGui::End();
		}
	}
}