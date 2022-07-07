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
#pragma region game
	const auto paintTraverseTarget = vfunc::getVFunc(interfaces::panel, paintTraverse::index);
	//const auto creteMoveTarget = vfunc::getVFunc(interfaces::clientMode, createMove::index);
	const auto drawModelTarget = vfunc::getVFunc(interfaces::modelRender, drawModel::index);
	const auto overrideViewTarget = vfunc::getVFunc(interfaces::clientMode, overrideView::index);
	const auto doPostScreenEffectsTarget = vfunc::getVFunc(interfaces::clientMode, doPostScreenEffects::index);
	const auto frameStageNotifyTarget = vfunc::getVFunc(interfaces::client, frameStageNotify::index);
	const auto lockCursorTarget = vfunc::getVFunc(interfaces::surface, lockCursor::index);
	const auto createMoveProxyTarget = vfunc::getVFunc(interfaces::client, proxyCreateMove::index);
	const auto sv_cheatsAddr = interfaces::cvar->findVar(XOR("sv_cheats"));
	const auto sv_cheatsTarget = vfunc::getVFunc(sv_cheatsAddr, sv_cheats::index);
	const auto unkFileSystemtarget = vfunc::getVFunc(interfaces::fileSystem, unknownFileSystem::index);
	const auto getUnverifiedFileHashesTarget = vfunc::getVFunc(interfaces::fileSystem, getUnverifiedFileHashes::index);
	const auto unkFileCheckTarget = vfunc::getVFunc(interfaces::fileSystem, unkFileCheck::index);
#pragma endregion

	const auto addrClient = utilities::patternScan(CLIENT_DLL, NEW_CHECK);
	const auto clientValidAddrTarget = reinterpret_cast<void*>(addrClient);

	const auto addrEngine = utilities::patternScan(ENGINE_DLL, NEW_CHECK);
	const auto engineValidAddrTarget = reinterpret_cast<void*>(addrEngine);

	const auto addrStudio = utilities::patternScan(STUDIORENDER_DLL, NEW_CHECK);
	const auto studioValidAddrTarget = reinterpret_cast<void*>(addrStudio);

	const auto addrMaterial = utilities::patternScan(MATERIAL_DLL, NEW_CHECK);
	const auto matertialValidAddrTarget = reinterpret_cast<void*>(addrMaterial);

	const auto addrIsusingDebugProps = utilities::patternScan(ENGINE_DLL, IS_USING_PROP_DEBUG);
	const auto isusingDebugPropsTarget = reinterpret_cast<void*>(addrIsusingDebugProps);

	const auto addrgetColorModulation = utilities::patternScan(MATERIAL_DLL, GET_COLOR_MODULATION);
	const auto getColorModulationTarget = reinterpret_cast<void*>(addrgetColorModulation);

	const auto addrextraBonesProccessing = utilities::patternScan(CLIENT_DLL, EXTRA_BONES_PROCCESSING);
	const auto extraBonesProccessingTarget = reinterpret_cast<void*>(addrextraBonesProccessing);

	const auto buildTr = utilities::patternScan(CLIENT_DLL, BUILD_TRANSFORMATIONS);
	const auto buildTrTarget = reinterpret_cast<void*>(buildTr);

	const auto particlesSimulation = utilities::patternScan(CLIENT_DLL, PARTICLE_SIMULATE);
	const auto particlesSimulationTarget = reinterpret_cast<void*>(particlesSimulation);

	const auto sendDatagramAddr = utilities::patternScan(ENGINE_DLL, SEND_DATAGRAM);
	const auto sendDatagramTarget = reinterpret_cast<void*>(sendDatagramAddr);

	const auto unkownOverviewMapAddr = utilities::patternScan(CLIENT_DLL, UNK_OVBERVIEWMAP);
	const auto unkownOverviewMapTarget = reinterpret_cast<void*>(unkownOverviewMapAddr);

#pragma region DX9
	const auto resetTarget = vfunc::getVFunc(interfaces::dx9Device, reset::index);
	const auto presentTagret = vfunc::getVFunc(interfaces::dx9Device, present::index);
	const auto drawIndexP = vfunc::getVFunc(interfaces::dx9Device, drawIndexedPrimitive::index);
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
	HOOK_SAFE(isusingDebugPropsTarget, isUsingStaticPropDebugModes);
	HOOK_SAFE(getColorModulationTarget, getColorModulation);
	HOOK_SAFE(resetTarget, reset);
	HOOK_SAFE(presentTagret, present);
	HOOK_SAFE(drawIndexP, drawIndexedPrimitive);
	HOOK_SAFE(lockCursorTarget, lockCursor);
	HOOK_SAFE(extraBonesProccessingTarget, doExtraBonesProcessing);
	HOOK_SAFE(buildTrTarget, buildTransformations);
	HOOK_SAFE(sv_cheatsTarget, sv_cheats);
	HOOK_SAFE(particlesSimulationTarget, particlesSimulations);
	HOOK_SAFE(sendDatagramTarget, sendDatagram);
	HOOK_SAFE(unkownOverviewMapTarget, unknownOverViewFun);
	HOOK_SAFE(unkFileSystemtarget, unknownFileSystem);
	HOOK_SAFE(getUnverifiedFileHashesTarget, getUnverifiedFileHashes);
	HOOK_SAFE(unkFileCheckTarget, unkFileCheck);

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