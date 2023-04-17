#include "wndproc.hpp"

#include <menu/GUI-ImGui/menu.hpp>
#include <config/vars.hpp>
#include <utilities/tools/tools.hpp>
#include <SDK/interfaces/interfaces.hpp>
#include <gamememory/memory.hpp>
#include <utilities/console/console.hpp>
#include <utilities/console/consoleDraw.hpp>
#include <cheats/classes/wndProcKeyHandler.hpp>

#include <MinHook.h>
#include <imgui_impl_win32.h>
#include <implot.h>

#include <mutex>

void wndProcSys::init()
{
	currentWindow = FindWindowA("Valve001", NULL);
	wndProcOriginal = reinterpret_cast<WNDPROC>(SetWindowLongW(currentWindow, GWL_WNDPROC, reinterpret_cast<LONG>(wndProcSys::wndproc)));
}

#include <SDK/InputSystem.hpp>
#include <config/key.hpp>
#include <utilities/inputSystem.hpp>
#include <menu/x88Menu/x88menu.hpp>
#include <render/backend/backend.hpp>

void wndProcSys::shutdown()
{
	SetWindowLongW(currentWindow, GWL_WNDPROC, reinterpret_cast<LONG>(wndProcSys::wndProcOriginal));
	memory::interfaces::iSystem->enableInput(true);
}

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);

LRESULT wndProcSys::wndproc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
	renderbackend::createContext(hwnd);

	KeysHandler::run(message, wparam);
	WndProcKeyHandler::updateAllKeys();

	memory::interfaces::iSystem->enableInput(!g_ImGuiMenu->isMenuActive());
	
	if(g_ImGuiMenu->isMenuActive() && ImGui_ImplWin32_WndProcHandler(hwnd, message, wparam, lparam))
		return TRUE;
	
	return LI_FN_CACHED(CallWindowProcA)(wndProcOriginal, hwnd, message, wparam, lparam);
}