#include "present.hpp"

#include <menu/GUI-ImGui/menu.hpp>
#include <render/render.hpp>
#include <render/backend/backend.hpp>
#include <utilities/console/console.hpp>
#include <utilities/res.hpp>
#include <utilities/tools/tools.hpp>
#include <gamememory/memory.hpp>

#include <imgui_impl_dx9.h>
#include <imgui_impl_win32.h>

#include <d3d9.h>
#include <mutex>

hooks::Present::value hooks::Present::hook(IDirect3DDevice9* device, RECT* srcRect, RECT* dstRect, HWND window, RGNDATA* region)
{
	// check if there is any context, needed when shutdown
	if (!ImGui::GetCurrentContext())
		return original(device, srcRect, dstRect, window, region);

	renderbackend::impl(device);

	// BEGIN DRAW
	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	{
		ImRender::present(ImGui::GetBackgroundDrawList());

		Storage::runs.run();
		ImNotify::handle();
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