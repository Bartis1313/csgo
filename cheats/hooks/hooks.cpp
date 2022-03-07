#include "hooks.hpp"
#include "../features/misc/events.hpp"
#include "../../dependencies/minhook/Minhook.h"
#include "../../SDK/structs/IDXandPaterrns.hpp"
#include "../../config/vars.hpp"
#include "../../cheats/menu/GUI-ImGui/menu.hpp"
#include "helpers/helper.hpp"
#include "../../utilities/renderer/renderer.hpp"
#include "../../dependencies/ImGui/imgui.h"
#include "../../dependencies/ImGui/imgui_impl_win32.h"
#include "../features/visuals/player.hpp"
#include "../features/visuals/world.hpp"
#include <format>

void hooks::init()
{
#pragma region game
	const auto paintTraverseTarget = vfunc::getVFunc(interfaces::panel, paintTraverse::index);
	//const auto creteMoveTarget = vfunc::getVFunc(interfaces::clientMode, createMove::index);
	const auto drawModelTarget = vfunc::getVFunc(interfaces::modelRender, drawModel::index);
	const auto overrideViewTarget = vfunc::getVFunc(interfaces::clientMode, overrideView::index);
	const auto doPostScreenEffectsTarget = vfunc::getVFunc(interfaces::clientMode, doPostScreenEffects::index);
	const auto frameStageNotifyTarget = vfunc::getVFunc(interfaces::client, frameStageNotify::index);
	const auto lockCursorTarget = vfunc::getVFunc(interfaces::surface, lockCursor::index);
	const auto createMoveProxyTarget = vfunc::getVFunc(interfaces::client, proxyCreateMove::index);
#pragma endregion

#pragma region checks
	const auto addrClient = utilities::patternScan(CLIENT_DLL, NEW_CHECK);
	const auto clientValidAddrTarget = reinterpret_cast<void*>(addrClient);
	const auto addrEngine = utilities::patternScan(ENGINE_DLL, NEW_CHECK);
	const auto engineValidAddrTarget = reinterpret_cast<void*>(addrEngine);
	const auto addrStudio = utilities::patternScan(STUDIORENDER_DLL, NEW_CHECK);
	const auto studioValidAddrTarget = reinterpret_cast<void*>(addrStudio);
	const auto addrMaterial = utilities::patternScan(MATERIAL_DLL, NEW_CHECK);
	const auto matertialValidAddrTarget = reinterpret_cast<void*>(addrMaterial);
#pragma endregion

#pragma region DX9
	const auto resetTarget = vfunc::getVFunc(interfaces::dx9Device, reset::index);
	const auto presentTagret = vfunc::getVFunc(interfaces::dx9Device, present::index);
	//const auto endTarget = vfunc::getVFunc(interfaces::dx9Device, endScene::index);
#pragma endregion

	hookHelper::initMinhook();

	//hookHelper::tryHook(creteMoveTarget, &createMove::hooked, hookHelper::ORIGINAL(createMove::original), XOR("createMove"));
	hookHelper::tryHook(createMoveProxyTarget, &proxyCreateMove::hooked, hookHelper::ORIGINAL(proxyCreateMove::original), XOR("createmoveProxy"));
	hookHelper::tryHook(paintTraverseTarget, &paintTraverse::hooked, hookHelper::ORIGINAL(paintTraverse::original), XOR("paintTraverse"));
	hookHelper::tryHook(drawModelTarget, &drawModel::hooked, hookHelper::ORIGINAL(drawModel::original), XOR("drawModel"));
	hookHelper::tryHook(overrideViewTarget, &overrideView::hooked, hookHelper::ORIGINAL(overrideView::original), "overrideView");
	hookHelper::tryHook(doPostScreenEffectsTarget, &doPostScreenEffects::hooked, hookHelper::ORIGINAL(doPostScreenEffects::original), XOR("doPostScreenEffects"));
	hookHelper::tryHook(frameStageNotifyTarget, &frameStageNotify::hooked, hookHelper::ORIGINAL(frameStageNotify::original), XOR("frameStageNotify"));
	hookHelper::tryHook(clientValidAddrTarget, &clientValidAddr::hooked, hookHelper::ORIGINAL(clientValidAddr::original), XOR("clientValidAddr"));
	hookHelper::tryHook(engineValidAddrTarget, &engineValidAddr::hooked, hookHelper::ORIGINAL(engineValidAddr::original), XOR("engineValidAddr"));
	hookHelper::tryHook(studioValidAddrTarget, &studioRenderValidAddr::hooked, hookHelper::ORIGINAL(studioRenderValidAddr::original), XOR("studioValidAddr"));
	hookHelper::tryHook(matertialValidAddrTarget, &materialSystemValidAddr::hooked, hookHelper::ORIGINAL(materialSystemValidAddr::original), XOR("matertialValidAddr"));
	hookHelper::tryHook(resetTarget, &reset::hooked, hookHelper::ORIGINAL(reset::original), XOR("reset"));
	hookHelper::tryHook(presentTagret, &present::hooked, hookHelper::ORIGINAL(present::original), XOR("present"));
	// use if endscene needed
	//hookHelper::tryHook(endTarget, &endScene::hooked, hookHelper::ORIGINAL(endScene::original), XOR("endscene"));
	hookHelper::tryHook(lockCursorTarget, &lockCursor::hooked, hookHelper::ORIGINAL(lockCursor::original), XOR("lock cursor"));

	events.init();

	hookHelper::checkAllHooks();

	LOG(LOG_INFO, XOR("hooks success"));
}

#pragma region wndproc
LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);

void hooks::wndProcSys::init()
{
#ifdef _DEBUG
	currentWindow = FindWindowA(XOR("Valve001"), NULL);
#else
	currentWindow = LF(FindWindowA).cached()(XOR("Valve001"), NULL);
#endif
	wndProcOriginal = reinterpret_cast<WNDPROC>(LF(SetWindowLongW)(currentWindow, GWL_WNDPROC, reinterpret_cast<LONG>(hooks::wndProcSys::wndproc)));
}

void hooks::wndProcSys::shutdown()
{
	LF(SetWindowLongW)(currentWindow, GWL_WNDPROC, reinterpret_cast<LONG>(hooks::wndProcSys::wndProcOriginal));
	interfaces::iSystem->enableInput(true);
}

LRESULT __stdcall hooks::wndProcSys::wndproc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
	// init content for imgui, once
	static auto bOnce = [=]()
	{
		ImGui::CreateContext();
		ImGui_ImplWin32_Init(hwnd);

		LOG(LOG_INFO, XOR("init for wndProc success"));

		return true;
	} ();

	// here, don't call getasynckeystate, it's another call for something that is already given by arg
	if (message == WM_KEYDOWN)
		if (LOWORD(wparam) == config.get<int>(vars.iKeyMenu))
			ImGuiMenu::isMenuActive = !ImGuiMenu::isMenuActive;

	interfaces::iSystem->enableInput(!ImGuiMenu::isMenuActive);

	// this needs to check it! this way we skip man flickering problems, maybe this is not like the best way
	if (ImGuiMenu::isMenuActive && ImGui_ImplWin32_WndProcHandler(hwnd, message, wparam, lparam))
		return TRUE;
#if _DEBUG
	return CallWindowProcA(wndProcOriginal, hwnd, message, wparam, lparam);
#else
	return LF(CallWindowProcA).cached()(wndProcOriginal, hwnd, message, wparam, lparam);
#endif
}
#pragma endregion

void hooks::shutdown()
{
	hookHelper::shutdownAllHooks();
}