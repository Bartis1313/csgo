#include "interfaces.hpp"

#include "ifc.hpp"
#include "../interfaceNode.hpp"
#include <gamememory/modules.hpp>

#include <utilities/tools/tools.hpp>
#include <utilities/tools/wrappers.hpp>
#include <utilities/console/console.hpp>
#include <gamememory/memory.hpp>

#include <Windows.h>
#include <d3d9.h>
#include <string>
#include <optional>
#include <mutex>

template <typename T>
static std::optional<Interface<T>> getInterface(const std::string_view moduleName, const std::string_view interfaceName)
{
	using fun = std::add_pointer_t<void* __cdecl(const char*, int*)>;
	auto capture = memory::exportVar<fun, "CreateInterface"_hasher>(moduleName);

	const auto addr = memory::Address<std::add_pointer_t<InterfacesNode>>{ reinterpret_cast<uintptr_t>(capture) };
	const auto esi = addr.rel(0x5, 0x6).deRef(memory::Dereference::TWICE);

	for (auto el = esi(); el; el = el->m_next)
	{
		if (std::string_view{ el->m_name }.starts_with(interfaceName))
		{
			return Interface<T>{ .base = static_cast<T>(el->m_createFn()), .module = moduleName };
		}
	}

	return std::nullopt;
}

// capture and log
// var - var you want to init
// _module - module name from the game, eg: engine.dll
// _interface - interface' name
#define CAP(var, _module, _interface) \
	if(auto val = getInterface<decltype(var)::value>(_module, _interface); val.has_value()) \
		var = val.value(); \
	else \
		throw std::runtime_error(std::format("Interface {} was nullptr", _interface)); \
	console::debug("found {} at addr: 0x{:X}", _interface, reinterpret_cast<uintptr_t>(var.base));

void memory::interfaces::init()
{
	CAP(engine, ENGINE_DLL, "VEngineClient0");
	CAP(panel, VGUI_DLL, "VGUI_Panel0");
	CAP(surface, VGUIMAT_DLL, "VGUI_Surface0");
	CAP(client, CLIENT_DLL, "VClient0");
	CAP(entList, CLIENT_DLL, "VClientEntityList0");
	CAP(cvar, VSTD_DLL, "VEngineCvar0");
	CAP(trace, ENGINE_DLL, "EngineTraceClient0");
	CAP(renderView, ENGINE_DLL, "VEngineRenderView0");
	CAP(matSys, MATERIAL_DLL, "VMaterialSystem0");
	CAP(modelInfo, ENGINE_DLL, "VModelInfoClient0");
	CAP(prediction, CLIENT_DLL, "VClientPrediction0");
	CAP(gameMovement, CLIENT_DLL, "GameMovement0");
	CAP(debugOverlay, ENGINE_DLL, "VDebugOverlay0");
	CAP(localize, LOCALIZE_DLL, "Localize_0");
	CAP(modelRender, ENGINE_DLL, "VEngineModel0");
	CAP(studioRender, STUDIORENDER_DLL, "VStudioRender0");
	CAP(eventManager, ENGINE_DLL, "GAMEEVENTSMANAGER002");
	CAP(efx, ENGINE_DLL, "VEngineEffects0");
	CAP(iSystem, INPUTSYSTEM_DLL, "InputSystemVersion0");
	CAP(effects, CLIENT_DLL, "IEffects0");
	CAP(fileSystem, FILESYS_DLL, "VFileSystem0");
	CAP(sound, ENGINE_DLL, "IEngineSoundClient0");
	CAP(mdlCache, DATACACHE_DLL, "MDLCache0");
	CAP(baseFileSystem, FILESYS_DLL, "VBaseFileSystem0");

	console::debug("interfaces success");
}