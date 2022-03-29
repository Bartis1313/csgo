#include "hooks.hpp"
#include "helpers/helper.hpp"

#include "../../SDK/structs/IDXandPaterrns.hpp"
#include "../../SDK/interfaces/interfaces.hpp"

#include "../../dependencies/minhook/Minhook.h"
#include "../../dependencies/ImGui/imgui.h"
#include "../../dependencies/ImGui/imgui_impl_win32.h"
#include "../../cheats/menu/GUI-ImGui/menu.hpp"

#include "../features/misc/events.hpp"
#include "../features/visuals/player.hpp"
#include "../features/visuals/world.hpp"

#include "../../config/vars.hpp"
#include "../../utilities/renderer/renderer.hpp"
#include "../../utilities/console/console.hpp"

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

	const auto addrIsusingDebugProps = utilities::patternScan(ENGINE_DLL, IS_USING_PROP_DEBUG);
	const auto IsusingDebugPropsTarget = reinterpret_cast<void*>(addrIsusingDebugProps);

	const auto addrgetColorModulation = utilities::patternScan(MATERIAL_DLL, GET_COLOR_MODULATION);
	const auto getColorModulationTarget = reinterpret_cast<void*>(addrgetColorModulation);
#pragma endregion

#pragma region DX9
	const auto resetTarget = vfunc::getVFunc(interfaces::dx9Device, reset::index);
	const auto presentTagret = vfunc::getVFunc(interfaces::dx9Device, present::index);
	//const auto endTarget = vfunc::getVFunc(interfaces::dx9Device, endScene::index);
#pragma endregion

	hookHelper::initMinhook();

#define HOOK_SAFE(target, hookStructName) \
hookHelper::tryHook(target, &hookStructName::hooked, hookHelper::ORIGINAL(hookStructName::original), XOR(#hookStructName));

	//HOOK_SAFE(creteMoveTarget, createMove);
	HOOK_SAFE(createMoveProxyTarget, proxyCreateMove);
	HOOK_SAFE(paintTraverseTarget, paintTraverse);
	HOOK_SAFE(drawModelTarget, drawModel);
	HOOK_SAFE(overrideViewTarget, overrideView);
	HOOK_SAFE(doPostScreenEffectsTarget, doPostScreenEffects);
	HOOK_SAFE(frameStageNotifyTarget, frameStageNotify);
	HOOK_SAFE(clientValidAddrTarget, clientValidAddr);
	HOOK_SAFE(engineValidAddrTarget, engineValidAddr);
	HOOK_SAFE(studioValidAddrTarget, studioRenderValidAddr);
	HOOK_SAFE(matertialValidAddrTarget, materialSystemValidAddr);
	HOOK_SAFE(IsusingDebugPropsTarget, isUsingStaticPropDebugModes);
	HOOK_SAFE(getColorModulationTarget, getColorModulation);
	HOOK_SAFE(resetTarget, reset);
	HOOK_SAFE(presentTagret, present);
	// use if endscene needed
	//HOOK_SAFE(endTarget, endScene);
	HOOK_SAFE(lockCursorTarget, lockCursor);

#undef HOOK_SAFE

	events.init();

	hookHelper::checkAllHooks();

	console.log(TypeLogs::LOG_INFO, XOR("hooks success"));
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

#include "../../SDK/InputSystem.hpp"

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

		console.log(TypeLogs::LOG_INFO, XOR("init for wndProc success"));

		return true;
	} ();

	// here, don't call getasynckeystate, it's another call for something that is already given by arg
	if (message == WM_KEYDOWN) // should run some key struct for this
	{
		if (LOWORD(wparam) == config.get<int>(vars.iKeyMenu))
			menu.changeActive();
		if (LOWORD(wparam) == config.get<int>(vars.iKeyConsoleLog))
			console.changeActiveLog();
	}

	interfaces::iSystem->enableInput(!menu.isMenuActive());

	// this needs to check it! this way we skip man flickering problems, maybe this is not like the best way
	if (menu.isMenuActive() && ImGui_ImplWin32_WndProcHandler(hwnd, message, wparam, lparam))
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