#include "setup.hpp"

#include "SEHcatch.hpp"

#include <config/vars.hpp>
#include <utilities/console/console.hpp>
#include <utilities/tools/tools.hpp>
#include <utilities/tools/wrappers.hpp>
#include <utilities/simpleTimer.hpp>
#include <utilities/netvars/netvars.hpp>
#include <utilities/renderer/renderer.hpp>
#include <SDK/interfaces/interfaces.hpp>
#include <menu/x88Menu/x88menu.hpp>
#include <menu/GUI-ImGui/menu.hpp>
#include <hooks/hooks.hpp>
#include <classes/base.hpp>
#include <features/events/events.hpp>
#include <features/discord/discord.hpp>
#include <features/cache/cache.hpp>
#include <game/globals.hpp>
#include <game/game.hpp>
#include <gamememory/memory.hpp>
#include <utilities/res.hpp>
#include <hooks/wndProc.hpp>

#include <discord_register.h>

#include <chrono>
#include <Windows.h>

DiscordPresence Setup::m_dc;

bool Setup::init(void* instance)
{
	// pass custom handler
	// commented due to false-positives thrown by steam server
	// they handle it somehow to prevent such crash, but exception pointers report it
	//AddVectoredExceptionHandler(TRUE, SEHcatch::memErrorCatch);

	console.init(XOR("CSGO DEBUG"), XOR("hack.log"));

	TimeCount initTimer{};

	try
	{
		config.init(
			XOR("default.cfg"), XOR("load.LOAD"),
			std::filesystem::path{ XOR("Bartis_internal") } / XOR("csgo"),
			XOR("utility"));
		g_Memory.init();
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
		LI_FN(MessageBoxA)(nullptr, err.what(), XOR("Runtime hack error"), MB_OK | MB_ICONERROR);
		LOG_ERR(XOR("Runtime hack error {}"), err.what());
		LI_FN(FreeLibraryAndExitThread)(static_cast<HMODULE>(instance), EXIT_FAILURE);
	}

	initTimer.end();
	LOG_INFO(XOR("main thread took {:.5f}s"), initTimer.getTime());
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
	LOG_INFO(XOR("Hack shutdown"));
	console.shutdown();
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