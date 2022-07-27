#include "hooks.hpp"
#include "helpers/helper.hpp"

#include "../../SDK/structs/IDXandPaterrns.hpp"
#include "../../SDK/ConVar.hpp"
#include "../../SDK/ICvar.hpp"
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
#define HOOK_SAFE_VFUNC(thisptr, hookStructName) \
hookHelper::tryHook(vfunc::getVFunc(thisptr, hookStructName::index), &hookStructName::hooked, \
	hookHelper::ORIGINAL(hookStructName::original), XOR(#hookStructName))

#define HOOK_SAFE_SIG(mod, mask, hookStructName) \
hookHelper::tryHook(reinterpret_cast<void*>(utilities::patternScan(mod, mask)), &hookStructName::hooked, \
	hookHelper::ORIGINAL(hookStructName::original), XOR(#hookStructName))

	hookHelper::initMinhook();

	HOOK_SAFE_SIG(CLIENT_DLL, NEW_CHECK, clientValidAddr);
	HOOK_SAFE_SIG(ENGINE_DLL, NEW_CHECK, engineValidAddr);
	HOOK_SAFE_SIG(STUDIORENDER_DLL, NEW_CHECK, studioRenderValidAddr);
	HOOK_SAFE_SIG(MATERIAL_DLL, NEW_CHECK, materialSystemValidAddr);
	HOOK_SAFE_SIG(ENGINE_DLL, IS_USING_PROP_DEBUG, isUsingStaticPropDebugModes);
	HOOK_SAFE_SIG(MATERIAL_DLL, GET_COLOR_MODULATION, getColorModulation);
	HOOK_SAFE_SIG(CLIENT_DLL, EXTRA_BONES_PROCCESSING, doExtraBonesProcessing);
	HOOK_SAFE_SIG(CLIENT_DLL, BUILD_TRANSFORMATIONS, buildTransformations);
	HOOK_SAFE_SIG(CLIENT_DLL, PARTICLE_SIMULATE, particlesSimulations);
	HOOK_SAFE_SIG(ENGINE_DLL, SEND_DATAGRAM, sendDatagram);
	HOOK_SAFE_SIG(CLIENT_DLL, UNK_OVERVIEWMAP, unknownOverViewFun);
	HOOK_SAFE_SIG(CLIENT_DLL, IS_DEPTH, isDepthOfField);

	HOOK_SAFE_VFUNC(interfaces::dx9Device, reset);
	HOOK_SAFE_VFUNC(interfaces::dx9Device, present);
	HOOK_SAFE_VFUNC(interfaces::dx9Device, drawIndexedPrimitive);
	HOOK_SAFE_VFUNC(interfaces::client, proxyCreateMove);
	HOOK_SAFE_VFUNC(interfaces::client, frameStageNotify);
	HOOK_SAFE_VFUNC(interfaces::panel, paintTraverse);
	HOOK_SAFE_VFUNC(interfaces::modelRender, drawModel);
	HOOK_SAFE_VFUNC(interfaces::clientMode, overrideView);
	HOOK_SAFE_VFUNC(interfaces::clientMode, doPostScreenEffects);
	HOOK_SAFE_VFUNC(interfaces::surface, lockCursor);
	HOOK_SAFE_VFUNC(interfaces::cvar->findVar(XOR("sv_cheats")), sv_cheats);
	HOOK_SAFE_VFUNC(interfaces::fileSystem, unknownFileSystem);
	HOOK_SAFE_VFUNC(interfaces::fileSystem, unkFileCheck);
	HOOK_SAFE_VFUNC(interfaces::fileSystem, getUnverifiedFileHashes);
	HOOK_SAFE_VFUNC(interfaces::viewRender, renderView);
	HOOK_SAFE_VFUNC(interfaces::engine, isHltv);
	HOOK_SAFE_VFUNC(interfaces::viewRender, screen2DEffect);

#undef HOOK_SAFE_VFUNC
#undef HOOK_SAFE_SIG

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
#include "../../config/key.hpp"
#include "../../utilities/inputSystem.hpp"
#include "../menu/x88Menu/x88menu.hpp"

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

	inputHandler.run(message, wparam);
	x88menu.handleKeys();

	auto& aimbotKey = config.getRef<Key>(vars.kAimbotKey);
	aimbotKey.update();
	auto& thirdpKey = config.getRef<Key>(vars.kThirdp);
	thirdpKey.update();
	auto& freeLookKey = config.getRef<Key>(vars.kFreeLook);
	freeLookKey.update();
	auto& freeCamKey = config.getRef<Key>(vars.kFreeCam);
	freeCamKey.update();
	auto& cameraKey = config.getRef<Key>(vars.kMirrorCam);
	cameraKey.update();

	if(config.get<Key>(vars.kMenu).isPressed())
		menu.changeActive();

	if (config.get<Key>(vars.kConsoleLog).isPressed())
		console.changeActiveLog();

	interfaces::iSystem->enableInput(!menu.isMenuActive());

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