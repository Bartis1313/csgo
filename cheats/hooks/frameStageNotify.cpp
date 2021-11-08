#include "hooks.hpp"
#include "../features/backtrack/backtrack.hpp"

void __stdcall hooks::frameStageNotify::hooked(int frame)
{
	static bool bOnce = []()
	{
		backtrack::init();
		return true;
	} ();
	
	// check for frame enums ALWAYS, it will be huge performance drop
	if(interfaces::engine->isInGame() && frame == FRAME_NET_UPDATE_END)
		backtrack::update();

	original(interfaces::client, frame);
}
