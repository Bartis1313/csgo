#include "interfaces.hpp"

#include <Windows.h>
#include <type_traits>
#include <d3d9.h>
#include <format>

#include "../structs/IDXandPaterrns.hpp"
#include "../../utilities/utilities.hpp"
#include "../../utilities/utilities.hpp"
#include "../../utilities/console/console.hpp"

template <typename T>
static T* getInterface(const std::string_view moduleName, const std::string_view interfaceName)
{
	// using fun = void* (*)(const char*, int*);
	using fun = void* __cdecl(const char*, int*);
	/*const auto capture = reinterpret_cast<fun>(
		LF(GetProcAddress)(LF(GetModuleHandleA)(moduleName), XOR("CreateInterface"))
		);*/
	const auto capture = reinterpret_cast<std::add_pointer_t<fun>>(
		LF(GetProcAddress)(LF(GetModuleHandleA)(moduleName.data()), XOR("CreateInterface"))
		);
	if (const auto ret = capture(interfaceName.data(), nullptr); ret != nullptr)
		return reinterpret_cast<T*>(ret);
	else
		throw std::runtime_error(FORMAT(XOR("Interface {} was nullptr"), interfaceName));

	return nullptr;
}

// capture and log
// var - var you want to init
// type - type for template
// _module - module name from the game, eg: engine.dll
// _interface - interface' name
#define CAPNLOG(var, type, _module, _interface) \
	var = getInterface<type>(_module, XOR(_interface)); \
	console.log(TypeLogs::LOG_INFO, FORMAT(XOR("found {} at addr: {:#0x}"), _interface, reinterpret_cast<uintptr_t>(var))); \
	
bool interfaces::init()
{
	CAPNLOG(engine, IVEngineClient, ENGINE_DLL, "VEngineClient014");
	CAPNLOG(panel, IPanel, VGUI_DLL, "VGUI_Panel009");
	CAPNLOG(surface, ISurface, VGUIMAT_DLL, "VGUI_Surface031");
	CAPNLOG(client, IBaseClientDLL, CLIENT_DLL, "VClient018");
	CAPNLOG(entList, IClientEntityList, CLIENT_DLL, "VClientEntityList003");
	CAPNLOG(cvar, ICvar, VSTD_DLL, "VEngineCvar007");
	CAPNLOG(trace, IEngineTrace, ENGINE_DLL, "EngineTraceClient004");
	CAPNLOG(renderView, IVRenderView, ENGINE_DLL, "VEngineRenderView014");
	CAPNLOG(matSys, IMaterialSystem, MATERIAL_DLL, "VMaterialSystem080");
	CAPNLOG(modelInfo, IVModelInfo, ENGINE_DLL, "VModelInfoClient004");
	CAPNLOG(prediction, IPrediction, CLIENT_DLL, "VClientPrediction001");
	CAPNLOG(gameMovement, CGameMovement, CLIENT_DLL, "GameMovement001");
	CAPNLOG(debugOverlay, IVDebugOverlay, ENGINE_DLL, "VDebugOverlay004");
	CAPNLOG(localize, ILocalize, LOCALIZE_DLL, "Localize_001");
	CAPNLOG(modelRender, IVModelRender, ENGINE_DLL, "VEngineModel016");
	CAPNLOG(studioRender, IVStudioRender, STUDIORENDER_DLL, "VStudioRender026");
	CAPNLOG(eventManager, IGameEventManager, ENGINE_DLL, "GAMEEVENTSMANAGER002");
	CAPNLOG(efx, IVEfx, ENGINE_DLL, "VEngineEffects001");
	CAPNLOG(iSystem, InputSystem, INPUTSYSTEM_DLL, "InputSystemVersion001");
	CAPNLOG(effects, IEffects, CLIENT_DLL, "IEffects001");
	CAPNLOG(fileSystem, IFileSystem, FILESYS_DLL, "VFileSystem017");

#undef CAPNLOG

	keyValuesSys = reinterpret_cast<KeyValuesSys*(__cdecl*)()>(LF(GetProcAddress)(LF(GetModuleHandleA)(VSTD_DLL), XOR("KeyValuesSystem")))();
	memAlloc = *reinterpret_cast<IMemAlloc**>(LF(GetProcAddress)(LF(GetModuleHandleA)(TIER_DLL), XOR("g_pMemAlloc")));
	
	globalVars = **reinterpret_cast<CGlobalVarsBase***>((*reinterpret_cast<uintptr_t**>(client))[0] + 0x1F);
	clientMode = **reinterpret_cast<ClientMode***>((*reinterpret_cast<uintptr_t**>(client))[10] + 0x5);
	beams = *reinterpret_cast<IViewRenderBeams**>(utilities::patternScan(CLIENT_DLL, BEAMS, 0x1));
	input = *reinterpret_cast<Input**>((*reinterpret_cast<uintptr_t**>(client))[16] + 0x1);
	glowManager = *reinterpret_cast<CGlowManager**>(utilities::patternScan(CLIENT_DLL, GLOWMANAGER, 0x3));
	weapon = *reinterpret_cast<IWeapon**>(utilities::patternScan(CLIENT_DLL, WEAPONDATA, 0x2));
	moveHelper = **reinterpret_cast<IMoveHelper***>(utilities::patternScan(CLIENT_DLL, MOVEHELPER, 0x2));
	resource = *reinterpret_cast<PlayerResource***>(utilities::patternScan(CLIENT_DLL, PLAYER_RESOURCE, 0x4));
	dx9Device = **reinterpret_cast<IDirect3DDevice9***>(utilities::patternScan(SHARED_API, DX9_DEVICE, 0x1));
	clientState = **reinterpret_cast<IClientState***>(utilities::patternScan(ENGINE_DLL, CLIENT_STATE, 0x1));
	gameRules = *reinterpret_cast<CGameRules**>(utilities::patternScan(CLIENT_DLL, GAME_RULES, 0x2));
	viewRender = **reinterpret_cast<IViewRender***>(utilities::patternScan(CLIENT_DLL, VIEW_RENDER, 0x2));

	using namespace gameFunctions;

	tesla = reinterpret_cast<tfn>(utilities::patternScan(CLIENT_DLL, FX_TESLA));
	dispatchEffect = reinterpret_cast<dfn>(utilities::patternScan(CLIENT_DLL, DISPATCH_EFFECT));

	console.log(TypeLogs::LOG_INFO, XOR("interfaces success"));
	done = true;
	return true;
}
