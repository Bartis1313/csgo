#include "backend.hpp"

#include "../render.hpp"
#include <menu/GUI-ImGui/menu.hpp>
#include <menu/x88Menu/x88menu.hpp>
#include <utilities/console/console.hpp>

#include <mutex>
#include <imgui_impl_dx9.h>

void renderbackend::impl(IDirect3DDevice9* device)
{
	static std::once_flag once;
	std::call_once(once, [device]
		{
			ImGui_ImplDX9_Init(device);
			ImRender::init(ImGui::GetIO());
			ImGuiMenu::setStyles();
			x88Menu::setStyles();
			console::debug("renderbackend::impl success");
		});
}

#include <imgui_impl_win32.h>
#include <implot.h>

void renderbackend::createContext(HWND hwnd)
{
	static std::once_flag once;
	std::call_once(once, [hwnd]()
		{
#ifdef SURFACE_RENDER
			SurfaceRender::init();
#endif
			ImGui::CreateContext();
			ImPlot::CreateContext();
			ImGui_ImplWin32_Init(hwnd);

			console::debug("renderbackend::createContext success");
		});
}

void renderbackend::shutdown()
{
	static std::once_flag once;
	std::call_once(once, []()
		{
			ImGui_ImplDX9_Shutdown();
			ImGui_ImplWin32_Shutdown();
			ImGui::DestroyContext();
			ImPlot::DestroyContext();
		});
}