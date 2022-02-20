#include "hooks.hpp"
#include "../../dependencies/ImGui/imgui_impl_dx9.h"
#include "../../dependencies/ImGui/imgui_impl_win32.h"
#include "../../dependencies/ImGui/imgui_stdlib.h"
#include "../../dependencies/ImGui/imgui.h"
#include "../../dependencies/ImGui/imgui_freetype.h"
#include "../menu/GUI-ImGui/menu.hpp"

long __stdcall hooks::present::hooked(IDirect3DDevice9* device, RECT* srcRect, RECT* dstRect, HWND window, RGNDATA* region)
{
	// should not happen, but when imgui has no possibility to render, just straight return clean original
	if (!ImGui::GetCurrentContext())
		return original(device, srcRect, dstRect, window, region);

	// will only run once to init imgui
	static auto initOnce = [=]()
	{
		ImGui_ImplDX9_Init(device);
		// place style init here too

		return true;
	} ();
	// start
	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	// draw example for test
	ImGuiMenu::example();
	
	// end
	ImGui::EndFrame();
	ImGui::Render();

	// pass data if it's valid
	if (SUCCEEDED(device->BeginScene()))
	{
		ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
		device->EndScene();
	}

	// call with changes
	return original(device, srcRect, dstRect, window, region);
}