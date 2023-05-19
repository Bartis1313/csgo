#include "wndproc.hpp"

#include <menu/GUI-ImGui/menu.hpp>
#include <config/vars.hpp>
#include <utilities/tools/tools.hpp>
#include <SDK/interfaces/interfaces.hpp>
#include <gamememory/memory.hpp>
#include <utilities/console/console.hpp>
#include <utilities/console/consoleDraw.hpp>

#include <MinHook.h>
#include <imgui_impl_win32.h>
#include <implot.h>

#include <mutex>

void hooks::wndProcSys::init()
{
	currentWindow = FindWindowA("Valve001", NULL);
	wndProcOriginal = reinterpret_cast<WNDPROC>(SetWindowLongW(currentWindow, GWL_WNDPROC, reinterpret_cast<LONG>(wndProcSys::wndproc)));
}

#include <SDK/InputSystem.hpp>
#include <config/key.hpp>
#include <utilities/inputSystem.hpp>
#include <menu/x88Menu/x88menu.hpp>
#include <render/backend/backend.hpp>

void hooks::wndProcSys::shutdown()
{
	SetWindowLongW(currentWindow, GWL_WNDPROC, reinterpret_cast<LONG>(wndProcSys::wndProcOriginal));
	memory::interfaces::iSystem->enableInput(true);
}

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);

LRESULT hooks::wndProcSys::wndproc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
	renderbackend::createContext(hwnd);

	inputSystem::run(message, wparam);
	Storage::runs.run();

	memory::interfaces::iSystem->enableInput(!ImGuiMenu::active);
	
	if(ImGuiMenu::active && ImGui_ImplWin32_WndProcHandler(hwnd, message, wparam, lparam))
		return TRUE;
	
	return CallWindowProcA(wndProcOriginal, hwnd, message, wparam, lparam);
}