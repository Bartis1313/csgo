#include "hooks.hpp"

#include "../../SDK/IVEngineClient.hpp"
#include "../../SDK/Enums.hpp"
#include "../../SDK/interfaces/interfaces.hpp"

#include "../features/backtrack/backtrack.hpp"
#include "../features/visuals/world.hpp"
#include "../globals.hpp"

void __stdcall hooks::frameStageNotify::hooked(int frame)
{
	static bool bOnce = []()
	{
		backtrack.init();
		return true;
	} ();

	world.skyboxLoad(frame, globals::isShutdown);
	if (frame == FRAME_RENDER_START)
		backtrack.update();
	// check for frame enums ALWAYS, it will be huge performance drop

	original(interfaces::client, frame);
}
