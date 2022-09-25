#include "interfaces.hpp"

#include <utilities/tools/tools.hpp>
#include <utilities/tools/wrappers.hpp>
#include <utilities/console/console.hpp>
#include <utilities/vfunc.hpp>
#include <gamememory/memory.hpp>

#include <Windows.h>
#include <d3d9.h>

template <typename T>
static T* getInterface(const std::string_view moduleName, const std::string_view interfaceName)
{
	using fun = void* (*)(const char*, int*);
	fun capture;
	LI_EXPORT(capture, reinterpret_cast<fun>, CreateInterface, g_Memory.getModule(moduleName));

	if (const auto ret = capture(interfaceName.data(), nullptr); ret != nullptr)
		return reinterpret_cast<T*>(ret);
			
	throw std::runtime_error(FORMAT(XOR("Interface {} was nullptr"), interfaceName));
}

// capture and log
// var - var you want to init
// type - type for template
// _module - module name from the game, eg: engine.dll
// _interface - interface' name
#define CAPNLOG(var, type, _module, _interface) \
	var = getInterface<type>(_module, XOR(_interface)); \
	console.log(TypeLogs::LOG_INFO, FORMAT(XOR("found {} at addr: 0x{:X}"), _interface, reinterpret_cast<uintptr_t>(var)));

#define LOG(var) \
	console.log(TypeLogs::LOG_INFO, FORMAT(XOR("found {} at addr: 0x{:X}"), #var, reinterpret_cast<uintptr_t>(var)));

#define ADDNLOG(var, mem) \
	var = mem; \
	console.log(TypeLogs::LOG_INFO, FORMAT(XOR("found {} at addr: 0x{:X}"), #var, reinterpret_cast<uintptr_t>(var)));

#define ENGINE_DLL					XOR("engine.dll")
#define CLIENT_DLL					XOR("client.dll")
#define VSTD_DLL					XOR("vstdlib.dll")
#define VGUI_DLL					XOR("vgui2.dll")
#define VGUIMAT_DLL					XOR("vguimatsurface.dll")
#define MATERIAL_DLL				XOR("materialsystem.dll")
#define LOCALIZE_DLL				XOR("localize.dll")
#define STUDIORENDER_DLL			XOR("studiorender.dll")
#define INPUTSYSTEM_DLL				XOR("inputsystem.dll")
#define SHARED_API					XOR("shaderapidx9.dll")
#define TIER_DLL					XOR("tier0.dll")
#define PANORAMA_DLL				XOR("panorama.dll")
#define FILESYS_DLL					XOR("filesystem_stdio.dll")
	
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

	LI_EXPORT_CDECL(keyValuesSys, reinterpret_cast<KeyValuesSys * (__cdecl*)()>, KeyValuesSystem, g_Memory.getModule(VSTD_DLL));
	LOG(keyValuesSys);
	LI_EXPORT(memAlloc, *reinterpret_cast<IMemAlloc**>, g_pMemAlloc, g_Memory.getModule(TIER_DLL));
	LOG(memAlloc);

	globalVars = **reinterpret_cast<CGlobalVarsBase***>((*reinterpret_cast<uintptr_t**>(client))[0] + 0x1F);
	LOG(globalVars);
	clientMode = **reinterpret_cast<ClientMode***>((*reinterpret_cast<uintptr_t**>(client))[10] + 0x5);
	LOG(clientMode);
	input = *reinterpret_cast<Input**>((*reinterpret_cast<uintptr_t**>(client))[16] + 0x1);
	LOG(input);

#undef LOG

	ADDNLOG(beams, g_Memory.m_beams());
	ADDNLOG(glowManager, g_Memory.m_glowManager());
	ADDNLOG(weapon, g_Memory.m_weaponInterface());
	ADDNLOG(moveHelper, g_Memory.m_moveHelper());
	ADDNLOG(resource, g_Memory.m_resourceInterface());
	ADDNLOG(dx9Device, g_Memory.m_dx9Device());
	ADDNLOG(clientState, g_Memory.m_clientState());
	ADDNLOG(gameRules, g_Memory.m_gameRules());
	ADDNLOG(viewRender, g_Memory.m_viewRender());

#undef ADDNLOG

	console.log(TypeLogs::LOG_INFO, XOR("interfaces success"));
	return true;
}
