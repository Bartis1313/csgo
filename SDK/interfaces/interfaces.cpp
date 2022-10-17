#include "interfaces.hpp"

#include "../interfaceNode.hpp"

#include <utilities/tools/tools.hpp>
#include <utilities/tools/wrappers.hpp>
#include <utilities/console/console.hpp>
#include <utilities/vfunc.hpp>
#include <gamememory/memory.hpp>

#include <Windows.h>
#include <d3d9.h>
#include <string>
#include <optional>

#define EXPORT(var, hash, _module) \
	var = ::li::detail::lazy_function<hash, decltype(var)>().in(g_Memory.getModule(_module));

#define EXPORT_LOG(var, hash, _module) \
	var = ::li::detail::lazy_function<hash, decltype(var)>().in(g_Memory.getModule(_module)); \
	LOG_INFO(XOR("found {} at addr: 0x{:X}"), #var, reinterpret_cast<uintptr_t>(var));

template <typename T>
static std::optional<T*> getInterface(const std::string_view moduleName, const std::string_view interfaceName)
{
	using fun = void* (*)(const char*, int*);
	fun capture;
	EXPORT(capture, "CreateInterface"_hash, moduleName);

	const auto addr = Memory::Address<InterfacesNode*>{ reinterpret_cast<uintptr_t>(capture) };
	const auto esi = addr.rel(0x5, 0x6).ref(Memory::Dereference::TWICE);

	for (auto el = esi(); el; el = el->m_next)
	{
		if (std::string_view{ el->m_name }.starts_with(interfaceName))
			return static_cast<T*>(el->m_createFn());
	}

	return std::nullopt;
}

#undef EXPORT

// capture and log
// var - var you want to init
// type - type for template
// _module - module name from the game, eg: engine.dll
// _interface - interface' name
#define CAP(var, type, _module, _interface) \
	if(auto val = getInterface<type>(_module, XOR(_interface)); val.has_value()) \
		var = val.value(); \
	else \
		throw std::runtime_error(FORMAT(XOR("Interface {} was nullptr"), _interface)); \
	LOG_INFO(XOR("found {} at addr: 0x{:X}"), _interface, reinterpret_cast<uintptr_t>(var));

#define ADD(var, mem) \
	var = mem; \
	LOG_INFO(XOR("found {} at addr: 0x{:X}"), #var, reinterpret_cast<uintptr_t>(var));

#define FROM_VFUNC(var, expression) \
	var = expression; \
	LOG_INFO(XOR("found {} at addr: 0x{:X}"), #var, reinterpret_cast<uintptr_t>(var));

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
	CAP(engine, IVEngineClient, ENGINE_DLL, "VEngineClient0");
	CAP(panel, IPanel, VGUI_DLL, "VGUI_Panel0");
	CAP(surface, ISurface, VGUIMAT_DLL, "VGUI_Surface0");
	CAP(client, IBaseClientDLL, CLIENT_DLL, "VClient0");
	CAP(entList, IClientEntityList, CLIENT_DLL, "VClientEntityList0");
	CAP(cvar, ICvar, VSTD_DLL, "VEngineCvar0");
	CAP(trace, IEngineTrace, ENGINE_DLL, "EngineTraceClient0");
	CAP(renderView, IVRenderView, ENGINE_DLL, "VEngineRenderView0");
	CAP(matSys, IMaterialSystem, MATERIAL_DLL, "VMaterialSystem0");
	CAP(modelInfo, IVModelInfo, ENGINE_DLL, "VModelInfoClient0");
	CAP(prediction, IPrediction, CLIENT_DLL, "VClientPrediction0");
	CAP(gameMovement, CGameMovement, CLIENT_DLL, "GameMovement0");
	CAP(debugOverlay, IVDebugOverlay, ENGINE_DLL, "VDebugOverlay0");
	CAP(localize, ILocalize, LOCALIZE_DLL, "Localize_0");
	CAP(modelRender, IVModelRender, ENGINE_DLL, "VEngineModel0");
	CAP(studioRender, IVStudioRender, STUDIORENDER_DLL, "VStudioRender0");
	CAP(eventManager, IGameEventManager, ENGINE_DLL, "GAMEEVENTSMANAGER002");
	CAP(efx, IVEfx, ENGINE_DLL, "VEngineEffects0");
	CAP(iSystem, InputSystem, INPUTSYSTEM_DLL, "InputSystemVersion0");
	CAP(effects, IEffects, CLIENT_DLL, "IEffects0");
	CAP(fileSystem, IFileSystem, FILESYS_DLL, "VFileSystem0");

#undef CAP

	EXPORT_LOG(keyValuesSys, "KeyValuesSystem"_hash, VSTD_DLL);
	EXPORT_LOG(memAlloc, "g_pMemAlloc"_hash, TIER_DLL);

#undef EXPORT_LOG

	FROM_VFUNC(globalVars, Memory::Address<CGlobalVarsBase*>{ vfunc::getVFunc(client, 0) }.add(0x1F).ref(Memory::Dereference::TWICE)());
	FROM_VFUNC(clientMode, Memory::Address<ClientMode*>{ vfunc::getVFunc(client, 10) }.add(0x5).ref(Memory::Dereference::TWICE)());
	FROM_VFUNC(input, Memory::Address<Input*>{ vfunc::getVFunc(client, 16) }.add(0x1).ref()());

#undef FROM_VFUNC

	ADD(beams, g_Memory.m_beams());
	ADD(glowManager, g_Memory.m_glowManager());
	ADD(weapon, g_Memory.m_weaponInterface());
	ADD(moveHelper, g_Memory.m_moveHelper());
	ADD(resource, g_Memory.m_resourceInterface());
	ADD(dx9Device, g_Memory.m_dx9Device());
	ADD(clientState, g_Memory.m_clientState());
	ADD(gameRules, g_Memory.m_gameRules());
	ADD(viewRender, g_Memory.m_viewRender());

#undef ADD

	LOG_INFO(XOR("interfaces success"));
	return true;
}