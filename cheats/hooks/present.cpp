#include "hooks.hpp"

#include <d3d9.h>

#include "../../dependencies/ImGui/imgui_impl_dx9.h"
#include "../../dependencies/ImGui/imgui_impl_win32.h"
#include "../../dependencies/ImGui/imgui.h"
#include "../menu/GUI-ImGui/menu.hpp"
#include "../menu/GUI-ImGui/background.hpp"

#include "../features/visuals/radar.hpp"
#include "../features/misc/misc.hpp"
#include "../features/visuals/world.hpp"
#include "../features/visuals/mirrorCam.hpp"
#include "../features/misc/freeCam.hpp"

#include "../../utilities/renderer/renderer.hpp"
#include "../../utilities/utilities.hpp"
#include "../../utilities/console/console.hpp"
#include "../../utilities/res.hpp"
#include "../../resource.h"

static bool isValidWindow()
{
	// sub window is better, for cs as they recently updated main window name
#ifdef _DEBUG
	if (auto window = FindWindowA("Valve001", NULL); GetForegroundWindow() != window)
		return false;
#else
	if (auto window = LF(FindWindowA).cached()(XOR("Valve001"), NULL); LF(GetForegroundWindow).cached()() != window)
		return false;
#endif
	return true;
}

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

		console.log(TypeLogs::LOG_INFO, XOR("init for present success"));

		return true;
	} ();

	if (!isValidWindow())
		return original(device, srcRect, dstRect, window, region);

	IDirect3DVertexDeclaration9* ppdecl;
	IDirect3DVertexShader9* ppshader;
	device->GetVertexDeclaration(&ppdecl);
	device->GetVertexShader(&ppshader);

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
		//imRender.drawImage(imdraw, res, 300, 300, 200, 200);
		imRender.renderPresent(imdraw);
		background.draw(imdraw);
		radar.run();
		misc.drawFpsPlot();
		misc.drawVelocityPlot();
		world.drawBombOverlay();
		mCam.draw();
		freeCam.drawInfo();
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

	device->SetVertexDeclaration(ppdecl);
	device->SetVertexShader(ppshader);

	return original(device, srcRect, dstRect, window, region);
}