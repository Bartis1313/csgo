#include "events.hpp"
#include "../../menu/vars.hpp"
#include "../../game.hpp"
#include "../visuals/visuals.hpp"

void Events::init()
{
	interfaces::eventManager->addListener(&Events::g(), XOR("player_footstep"));
	interfaces::eventManager->addListener(&Events::g(), XOR("player_death"));
	interfaces::eventManager->addListener(&Events::g(), XOR("round_start"));
	LOG("[init] events hooked\n");
}

// TODO: add more events and shots fired global intiger for our local player
// this will be needed to display current accuracy since there is no netvar or anything for this as I could not find
void Events::FireGameEvent(IGameEvent* event)
{
	if (!strcmp(event->getName(), XOR("player_footstep")))
	{
		Esp::drawSound(event);
	}
	else if (!strcmp(event->getName(), XOR("player_death")))
	{
		// testing
		interfaces::console->ConsolePrintf("Died\n");
	}
	else if (!strcmp(event->getName(), XOR("round_start")))
	{
		
	}
}

void Events::shutdown()
{
	interfaces::eventManager->removeListener(&Events::g());

}