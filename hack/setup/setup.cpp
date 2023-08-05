#include "setup.hpp"

#include <config/vars.hpp>
#include <utilities/console/console.hpp>
#include <utilities/tools/tools.hpp>
#include <utilities/tools/wrappers.hpp>
#include <utilities/simpleTimer.hpp>
#include <SDK/helpers/netvars.hpp>
#include <render/backend/backend.hpp>
#include <SDK/interfaces/interfaces.hpp>
#include <menu/x88Menu/x88menu.hpp>
#include <menu/GUI-ImGui/menu.hpp>
#include <cheats/hooks/hooks.hpp>
#include <cheats/features/events/events.hpp>
#include <cheats/features/cache/cache.hpp>
#include <cheats/game/globals.hpp>
#include <cheats/game/game.hpp>
#include <gamememory/memory.hpp>
#include <utilities/res.hpp>
#include <cheats/hooks/wndProc.hpp>

#include <cheats/helper/initable.hpp>
#include <cheats/helper/shutdownable.hpp>

#include <Windows.h>

bool setup::init(void* instance)
{
	console::setLogger("CSGO DEBUG", "hack.log");
	console::info("Hack start, {} {}", __DATE__, __TIME__);

	TimeCount initTimer{};

	HACK_TRY
	{
		config::init();
		memory::init();
		game::init();
		hooks::init();
	}
	HACK_CATCH(const HACK_CATCH_TYPE& err)
	{
#ifdef WANT_EXCEPTIONS
		api::messageBox(err.what(), "Runtime hack error");
		console::error("Runtime hack error {}", err.what());
		FreeLibraryAndExitThread(static_cast<HMODULE>(instance), EXIT_FAILURE);
#endif
	}

	initTimer.end();
	console::info("Main thread took {:.5f}s", initTimer.getTime());

	return true;
}

void setup::shutdown(void* instance)
{
	// already executed
	if (globals::isShutdown)
		return;

	globals::isShutdown = true;

	game::shutdown();
	hooks::shutdown();
	// doing this after hooks shutdown very unlikely will throw those ctx nullptr errors
	renderbackend::shutdown();
	console::info("Hack shutdown");
	console::shutdown();

	_CRT_INIT(static_cast<HINSTANCE>(instance), DLL_PROCESS_DETACH, nullptr);
	FreeLibraryAndExitThread(static_cast<HMODULE>(instance), EXIT_SUCCESS); // crts may crash, game threadings etc...
}

void setup::looper(void* instance)
{
	while (!vars::keys->panic.isPressed())
	{
		Sleep(1000);
	}

	shutdown(instance);
}