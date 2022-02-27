#include "hooks.hpp"
#include "../../dependencies/ImGui/imgui_impl_dx9.h"
#include "../../dependencies/ImGui/imgui_impl_win32.h"
#include "../../dependencies/ImGui/imgui.h"
#include "../menu/GUI-ImGui/menu.hpp"
#include "../../utilities/renderer/renderer.hpp"
#include "../features/visuals/player.hpp"
#include "../features/visuals/world.hpp"

long __stdcall hooks::present::hooked(IDirect3DDevice9* device, RECT* srcRect, RECT* dstRect, HWND window, RGNDATA* region)
{
	// check if there is any context, needed when shutdown
	if (!ImGui::GetCurrentContext())
		return original(device, srcRect, dstRect, window, region);

	static auto bOnce = [=]()
	{
		ImGui_ImplDX9_Init(device);
		// style, colors, ini file etc.
		ImGuiMenu::init();

		LOG(LOG_INFO, XOR("init for present success"));

		return true;
	} ();

	// BEGIN DRAW
	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	imRender.m_drawing = ImGui::GetBackgroundDrawList();
	// PUT CONTENT TO DRAW
	{
		world::drawZeusRange();

		ImGuiMenu::draw();
	}
	// END DRAW
	ImGui::EndFrame();
	ImGui::Render();

	// CHECK RENDER
	if (device->BeginScene() == D3D_OK)
	{
		ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
		device->EndScene();
	}

	return original(device, srcRect, dstRect, window, region);
}