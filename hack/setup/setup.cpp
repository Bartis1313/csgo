#include "setup.hpp"

#include "SEHcatch.hpp"

#include <config/vars.hpp>
#include <utilities/console/console.hpp>
#include <utilities/tools/tools.hpp>
#include <utilities/tools/wrappers.hpp>
#include <utilities/simpleTimer.hpp>
#include <SDK/helpers/netvars.hpp>
#include <render/render.hpp>
#include <SDK/interfaces/interfaces.hpp>
#include <menu/x88Menu/x88menu.hpp>
#include <menu/GUI-ImGui/menu.hpp>
#include <cheats/hooks/hooks.hpp>
#include <cheats/classes/base.hpp>
#include <cheats/features/events/events.hpp>
#include <cheats/features/discord/discord.hpp>
#include <cheats/features/cache/cache.hpp>
#include <cheats/game/globals.hpp>
#include <cheats/game/game.hpp>
#include <gamememory/memory.hpp>
#include <utilities/res.hpp>
#include <cheats/hooks/wndProc.hpp>

#include <discord_register.h>

#include <chrono>
#include <Windows.h>

DiscordPresence Setup::m_dc;

bool Setup::init(void* instance)
{
	//AddVectoredExceptionHandler(0, handler);

	TimeCount initTimer{};
	// might need to rewrite this later with setting those names
	config.setFolder(std::filesystem::path{ "Bartis_internal" } / "csgo");
	console::setLogger("CSGO DEBUG", "hack.log");
	try
	{
		config.init("default.cfg", "load.LOAD", "utility");
		memory::init();
		netvarMan.init();
		wndProcSys::init();
		BaseHack::initAll();
		surfaceRender.init();
		x88menu.init();
		hooks::init();
		EntityCache::init();
	}
	catch (const std::exception& err)
	{
		LI_FN(MessageBoxA)(nullptr, err.what(), "Runtime hack error", MB_OK | MB_ICONERROR);
		console::error("Runtime hack error {}", err.what());
		LI_FN(FreeLibraryAndExitThread)(static_cast<HMODULE>(instance), EXIT_FAILURE);
	}

	initTimer.end();
	console::debug("main thread took {:.5f}s", initTimer.getTime());
	m_inited = true;

	return true;
}

void Setup::shutdown(void* instance)
{
	globals::isShutdown = true;

	//RemoveVectoredExceptionHandler(globals::instance);
	wndProcSys::shutdown();
	BaseHack::shutdownAll();
	Resource::destroyAll();
	events::shutdown();
	hooks::shutdown();
	menu.shutdown();
	console::info("Hack shutdown");
	console::shutdown();
	m_dc.shutdown();

	LI_FN(FreeLibraryAndExitThread)(static_cast<HMODULE>(instance), EXIT_SUCCESS);
}

void Setup::looper(void* instance)
{
	using namespace std::chrono_literals;

	m_dc.init();
	while (!vars::keys->panic.isPressed())
	{
		if (m_inited)
		{
			m_dc.run();
			std::this_thread::sleep_for(1s);
		}
	}

	shutdown(instance);
}