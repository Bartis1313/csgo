#include "interfaces.hpp"

#include "ifc.hpp"
#include "../interfaceNode.hpp"
#include <gamememory/modules.hpp>

#include <utilities/tools/tools.hpp>
#include <utilities/tools/wrappers.hpp>
#include <utilities/console/console.hpp>
#include <utilities/utilities.hpp>
#include <utilities/simpleTimer.hpp>
#include <config/config.hpp>
#include <gamememory/memory.hpp>

#include <Windows.h>
#include <d3d9.h>
#include <string>
#include <optional>

#ifdef CAPTURE_INTERFACES
static std::unordered_map<std::string, std::vector<std::pair<std::string, void*>>> allInterfaces;

static void initInterfacesAndDump()
{
	auto file = std::ofstream{ config::getHackPath() / "interfacesDump.txt" };
	file << std::format("Interfaces from: {}", utilities::getTime()) << "\n\n";

	TimeCount timer{};

	const auto mainPath = std::filesystem::current_path();

	for (const auto& entry : std::filesystem::directory_iterator{ mainPath / "bin" })
	{
		if (entry.is_regular_file())
		{
			if (const auto mod = entry.path().filename(); mod.extension() == ".dll")
			{
				using fun = std::add_pointer_t<void* __cdecl(const char*, int*)>;
				// not using by export, because it's all contexpr
				auto capture = memory::Address<void*>{ GetProcAddress(GetModuleHandleA(mod.string().c_str()), "CreateInterface") }.cast<fun>();
				if (!capture.getAddr())
					continue;

				const auto addr = memory::Address<std::add_pointer_t<InterfacesNode>>{ capture.getAddr() };
				const auto esi = addr.rel(0x5, 0x6).deRef(memory::Dereference::TWICE);

				for (auto el = esi(); el; el = el->m_next)
				{
					if (const auto name = std::string_view{ el->m_name }; !name.empty())
					{
						allInterfaces[mod.string()].emplace_back(std::make_pair(name, el->m_createFn()));
					}
				}
			}
		}
	}

	for (const auto& entry : std::filesystem::directory_iterator{ mainPath / "csgo" / "bin" })
	{
		if (entry.is_regular_file())
		{
			if (const auto mod = entry.path().filename(); mod.extension() == ".dll")
			{
				using fun = std::add_pointer_t<void* __cdecl(const char*, int*)>;
				auto capture = memory::Address<void*>{ GetProcAddress(GetModuleHandleA(mod.string().c_str()), "CreateInterface") }.cast<fun>();
				if (!capture.getAddr())
					continue;

				const auto addr = memory::Address<std::add_pointer_t<InterfacesNode>>{ capture.getAddr() };
				const auto esi = addr.rel(0x5, 0x6).deRef(memory::Dereference::TWICE);

				for (auto el = esi(); el; el = el->m_next)
				{
					if (const auto name = std::string_view{ el->m_name }; !name.empty())
					{
						allInterfaces[mod.string()].emplace_back(std::make_pair(name, el->m_createFn()));
					}
				}
			}
		}
	}

	timer.end();

	size_t count{ 0U };
	for (const auto [mod, detail] : allInterfaces)
	{
		file << mod << '\n';
		for (const auto [dllname, addr] : detail)
		{
			file << '\t' << dllname << '\n';
			++count;
		}
		file << '\n';
	}

	file << '\n' << std::format("Found {} interfaces\nFinished in {:.2f} secs", count, timer.getTime());
}
#endif

#ifdef CAPTURE_INTERFACES
template<typename T>
static void findInterface(Interface<T>& ifc, const std::string_view name)
{
	bool found{ false };
	for (const auto [mod, detail] : allInterfaces)
	{
		for (const auto [dllname, addr] : detail)
		{
			if (dllname.starts_with(name))
			{
				ifc = Interface<T>{ .base = static_cast<T>(addr) };
				found = true;
				break;
			}
		}
	}

	assert(found);
}
#else
template <typename T>
static std::optional<Interface<T>> findInterface(const std::string_view moduleName, const std::string_view interfaceName)
{
	using fun = std::add_pointer_t<void* __cdecl(const char*, int*)>;
	auto capture = memory::byExport<"CreateInterface"_hasher>(moduleName).cast<fun>();

	const auto addr = memory::Address<std::add_pointer_t<InterfacesNode>>{ capture.getAddr() };
	const auto esi = addr.rel(0x5, 0x6).deRef(memory::Dereference::TWICE);

	for (auto el = esi(); el; el = el->m_next)
	{
		if (std::string_view{ el->m_name }.starts_with(interfaceName))
		{
			return Interface<T>{.base = static_cast<T>(el->m_createFn()), .module = moduleName };
		}
	}

	return std::nullopt;
}
#endif

// capture and log
// var - var you want to init
// _module - module name from the game, eg: engine.dll
// _interface - interface' name
#define CAP(var, _module, _interface) \
	if(auto val = findInterface<decltype(var)::value>(_module, _interface); val.has_value()) \
		var = val.value(); \
	else \
		throw std::runtime_error(std::format("Interface {} was nullptr", _interface)); \
	console::debug("found {} at addr: 0x{:X}", _interface, reinterpret_cast<uintptr_t>(var.base));

void memory::interfaces::init()
{
#ifdef CAPTURE_INTERFACES
	initInterfacesAndDump();

	findInterface(engine, "VEngineClient0");
	findInterface(panel, "VGUI_Panel0");
	findInterface(surface, "VGUI_Surface0");
	findInterface(client, "VClient0");
	findInterface(entList, "VClientEntityList0");
	findInterface(cvar, "VEngineCvar0");
	findInterface(trace, "EngineTraceClient0");
	findInterface(renderView, "VEngineRenderView0");
	findInterface(matSys, "VMaterialSystem0");
	findInterface(modelInfo, "VModelInfoClient0");
	findInterface(prediction, "VClientPrediction0");
	findInterface(gameMovement, "GameMovement0");
	findInterface(debugOverlay, "VDebugOverlay0");
	findInterface(localize,"Localize_0");
	findInterface(modelRender, "VEngineModel0");
	findInterface(studioRender, "VStudioRender0");
	findInterface(eventManager, "GAMEEVENTSMANAGER002");
	findInterface(efx, "VEngineEffects0");
	findInterface(iSystem, "InputSystemVersion0");
	findInterface(effects, "IEffects0");
	findInterface(fileSystem, "VFileSystem0");
	findInterface(sound, "IEngineSoundClient0");
	findInterface(mdlCache, "MDLCache0");
	findInterface(baseFileSystem, "VBaseFileSystem0");
#else
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
#endif

	console::debug("interfaces success");
}