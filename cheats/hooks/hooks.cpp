#include "hooks.hpp"
#include "helpers/helper.hpp"

#include <SDK/structs/indexes.hpp>
#include <SDK/ConVar.hpp>
#include <SDK/ICvar.hpp>
#include <SDK/interfaces/interfaces.hpp>
#include <dependencies/minhook/Minhook.h>
#include <dependencies/ImGui/imgui.h>
#include <dependencies/ImGui/imgui_impl_win32.h>
#include <menu/GUI-ImGui/menu.hpp>
#include <config/vars.hpp>
#include <utilities/renderer/renderer.hpp>
#include <utilities/console/console.hpp>
#include <utilities/tools/tools.hpp>
#include <gamememory/memory.hpp>

void hooks::init()
{
#define HOOK_SAFE_VFUNC(thisptr, hookStructName) \
hookHelper::tryHook(vfunc::getVFunc(thisptr, hookStructName::index), &hookStructName::hooked, \
	hookHelper::ORIGINAL(hookStructName::original), XOR(#hookStructName))

#define HOOK_SAFE_SIG(target, hookStructName) \
hookHelper::tryHook(target, &hookStructName::hooked, \
	hookHelper::ORIGINAL(hookStructName::original), XOR(#hookStructName))

	hookHelper::initMinhook();

	HOOK_SAFE_SIG(g_Memory.m_clientValidAddr(), clientValidAddr);
	HOOK_SAFE_SIG(g_Memory.m_enginevalidAddr(), engineValidAddr);
	HOOK_SAFE_SIG(g_Memory.m_studioRenderValidAddr(), studioRenderValidAddr);
	HOOK_SAFE_SIG(g_Memory.m_materialSysValidAddr(), materialSystemValidAddr);
	HOOK_SAFE_SIG(g_Memory.m_isUsingPropDebug(), isUsingStaticPropDebugModes);
	HOOK_SAFE_SIG(g_Memory.m_getColorModulation(), getColorModulation);
	HOOK_SAFE_SIG(g_Memory.m_extraBonesProcessing(), doExtraBonesProcessing);
	HOOK_SAFE_SIG(g_Memory.m_buildTransformations(), buildTransformations);
	HOOK_SAFE_SIG(g_Memory.m_particleSimulate(), particlesSimulations);
	HOOK_SAFE_SIG(g_Memory.m_sendDataGram(), sendDatagram);
	HOOK_SAFE_SIG(g_Memory.m_unkOverviewMap(), unknownOverViewFun);
	HOOK_SAFE_SIG(g_Memory.m_isDepth(), isDepthOfField);
	HOOK_SAFE_SIG(g_Memory.m_fxBlood(), fxBlood);

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
	HOOK_SAFE_VFUNC(interfaces::viewRender, screen2DEffect);
	HOOK_SAFE_VFUNC(interfaces::engine, isHltv);

#undef HOOK_SAFE_VFUNC
#undef HOOK_SAFE_SIG

	hookHelper::checkAllHooks();

	console.log(TypeLogs::LOG_INFO, XOR("hooks success"));
}

#pragma region wndproc
LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);

void hooks::wndProcSys::init()
{
	currentWindow = LI_FN_CACHED(FindWindowA)(XOR("Valve001"), NULL);
	wndProcOriginal = reinterpret_cast<WNDPROC>(LI_FN(SetWindowLongW)(currentWindow, GWL_WNDPROC, reinterpret_cast<LONG>(hooks::wndProcSys::wndproc)));
}

#include <SDK/InputSystem.hpp>
#include <config/key.hpp>
#include <utilities/inputSystem.hpp>
#include <menu/x88Menu/x88menu.hpp>

void hooks::wndProcSys::shutdown()
{
	LI_FN(SetWindowLongW)(currentWindow, GWL_WNDPROC, reinterpret_cast<LONG>(hooks::wndProcSys::wndProcOriginal));
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

	return LI_FN_CACHED(CallWindowProcA)(wndProcOriginal, hwnd, message, wparam, lparam);
}
#pragma endregion

void hooks::shutdown()
{
	hookHelper::shutdownAllHooks();
}