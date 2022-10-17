#include "hooks.hpp"

#include "../classes/renderableToPresent.hpp"

#include <dependencies/ImGui/imgui_impl_dx9.h>
#include <dependencies/ImGui/imgui_impl_win32.h>
#include <dependencies/ImGui/imgui.h>
#include <menu/GUI-ImGui/menu.hpp>
#include <menu/GUI-ImGui/background.hpp>
#include <utilities/renderer/renderer.hpp>
#include <utilities/console/console.hpp>
#include <utilities/res.hpp>

#include <d3d9.h>

long __stdcall hooks::present::hooked(IDirect3DDevice9* device, RECT* srcRect, RECT* dstRect, HWND window, RGNDATA* region)
{
	// check if there is any context, needed when shutdown
	if (!ImGui::GetCurrentContext())
		return original(device, srcRect, dstRect, window, region);

	static auto bOnce = [=]()
	{
		ImGui_ImplDX9_Init(device);
		// style, colors, ini file etc.
		menu.init();

		LOG_INFO(XOR("init for present success"));

		return true;
	} ();

	static Resource res{ IDB_PNG1, "PNG" };

	// BEGIN DRAW
	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	// anything that is responsible by imgui to be drawn, not including the game's w2s
	{
		menu.draw();
		console.drawLog();
		const auto imdraw = ImGui::GetBackgroundDrawList();
		imRender.renderPresent(imdraw);
		background.draw(imdraw);
		RenderablePresentType::runAll();
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