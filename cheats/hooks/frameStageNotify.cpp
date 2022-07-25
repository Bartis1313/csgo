#include "hooks.hpp"

#include "../../SDK/IVEngineClient.hpp"
#include "../../SDK/Enums.hpp"
#include "../../SDK/interfaces/interfaces.hpp"

#include "../features/backtrack/backtrack.hpp"
#include "../features/visuals/world.hpp"
#include "../features/misc/misc.hpp"
#include "../globals.hpp"

void __stdcall hooks::frameStageNotify::hooked(int frame)
{
	static bool bOnce = []()
	{
		backtrack.init();
		return true;
	} ();

	world.weather(frame);
	backtrack.update(frame);
	world.skyboxLoad(frame, globals::isShutdown);
	// add bind to csgo yourself. This is useless
	//world.removeBloodSpray(frame);
	world.removeSmoke(frame);
	misc.flashLight(frame);

	original(interfaces::client, frame);
}
