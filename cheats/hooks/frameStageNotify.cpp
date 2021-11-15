#include "hooks.hpp"
#include "../features/backtrack/backtrack.hpp"
#include "../features/visuals/world.hpp"

void __stdcall hooks::frameStageNotify::hooked(int frame)
{
	static bool bOnce = []()
	{
		backtrack::init();
		return true;
	} ();

	world::skyboxLoad(frame);
	// check for frame enums ALWAYS, it will be huge performance drop
	if(interfaces::engine->isInGame() && frame == FRAME_RENDER_START)
		backtrack::update();

	original(interfaces::client, frame);
}
