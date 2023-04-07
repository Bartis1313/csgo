#include "hooks.hpp"

#include "../classes/renderableToPresent.hpp"

#include <menu/GUI-ImGui/menu.hpp>
#include <menu/GUI-ImGui/background.hpp>
#include <render/render.hpp>
#include <utilities/console/console.hpp>
#include <utilities/res.hpp>
#include <utilities/tools/tools.hpp>

#include <imgui_impl_dx9.h>
#include <imgui_impl_win32.h>

#include <d3d9.h>
#include <mutex>

hooks::present::value D3DAPI hooks::present::hooked(IDirect3DDevice9* device, RECT* srcRect, RECT* dstRect, HWND window, RGNDATA* region)
{
	// check if there is any context, needed when shutdown
	if (!ImGui::GetCurrentContext())
		return original(device, srcRect, dstRect, window, region);

	static std::once_flag once;
	std::call_once(once, [device]
		{
			ImGui_ImplDX9_Init(device);
			// style, colors, ini file etc.
			menu.init();

			console::debug("init for present success");
		});

	static Resource res{ IDB_PNG1, "PNG" };

	// BEGIN DRAW
	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	{
		menu.draw();

		imRender.renderPresent(ImGui::GetBackgroundDrawList());

		RenderablePresentType::runAll();
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