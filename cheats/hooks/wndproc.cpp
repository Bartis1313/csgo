#include "wndproc.hpp"

#include <dependencies/minhook/Minhook.h>
#include <dependencies/ImGui/imgui.h>
#include <dependencies/ImGui/imgui_impl_win32.h>
#include <menu/GUI-ImGui/menu.hpp>
#include <config/vars.hpp>
#include <utilities/tools/tools.hpp>
#include <SDK/interfaces/interfaces.hpp>
#include <gamememory/memory.hpp>
#include <utilities/console/console.hpp>
#include <utilities/console/consoleDraw.hpp>

LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);

void wndProcSys::init()
{
	currentWindow = LI_FN_CACHED(FindWindowA)(XOR("Valve001"), NULL);
	wndProcOriginal = reinterpret_cast<WNDPROC>(LI_FN(SetWindowLongW)(currentWindow, GWL_WNDPROC, reinterpret_cast<LONG>(wndProcSys::wndproc)));
}

#include <SDK/InputSystem.hpp>
#include <config/key.hpp>
#include <utilities/inputSystem.hpp>
#include <menu/x88Menu/x88menu.hpp>

void wndProcSys::shutdown()
{
	LI_FN(SetWindowLongW)(currentWindow, GWL_WNDPROC, reinterpret_cast<LONG>(wndProcSys::wndProcOriginal));
	memory::interfaces::iSystem->enableInput(true);
}

LRESULT wndProcSys::wndproc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
	// init content for imgui, once
	static auto bOnce = [=]()
	{
		ImGui::CreateContext();
		ImGui_ImplWin32_Init(hwnd);

		LOG_INFO(XOR("init for wndProc success"));

		return true;
	} ();

	inputHandler.run(message, wparam);
	x88menu.handleKeys();

	vars::keys->aimbot.update();
	vars::keys->thirdP.update();
	vars::keys->freeLook.update();
	vars::keys->freeCam.update();
	vars::keys->mirrorCam.update();

	if (vars::keys->menu.isPressed())
		menu.changeActive();

	if (vars::keys->console.isPressed())
		g_LogDrawer->handleKeys();

	memory::interfaces::iSystem->enableInput(!menu.isMenuActive());
	
	if(menu.isMenuActive() && ImGui_ImplWin32_WndProcHandler(hwnd, message, wparam, lparam))
		return TRUE;
	
	return LI_FN_CACHED(CallWindowProcA)(wndProcOriginal, hwnd, message, wparam, lparam);
}