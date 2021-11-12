#include "events.hpp"
#include "../../menu/vars.hpp"
#include "../../game.hpp"
#include "../visuals/visuals.hpp"
#include "../../../SDK/IGameEvent.hpp"

void Events::init() const
{
	interfaces::eventManager->addListener(&Events::g(), XOR("player_footstep"));
	interfaces::eventManager->addListener(&Events::g(), XOR("player_death"));
	interfaces::eventManager->addListener(&Events::g(), XOR("round_start"));
	LOG(LOG_INFO, "events hooked\n");
}

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

void Events::shutdown() const
{
	interfaces::eventManager->removeListener(&Events::g());
}