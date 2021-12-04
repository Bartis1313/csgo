#include "events.hpp"
#include "../../menu/vars.hpp"
#include "../../game.hpp"
#include "../visuals/player.hpp"
#include "../../../SDK/IGameEvent.hpp"
#include "misc.hpp"
#include "../../globals.hpp"

void Events::init() const
{
	interfaces::eventManager->addListener(&Events::g(), XOR("player_footstep"));
	interfaces::eventManager->addListener(&Events::g(), XOR("player_death"));
	interfaces::eventManager->addListener(&Events::g(), XOR("round_start"));
	interfaces::eventManager->addListener(&Events::g(), XOR("player_hurt"));
	interfaces::eventManager->addListener(&Events::g(), XOR("weapon_fire"));
	LOG(LOG_INFO, "events hooked\n");
}

void Events::FireGameEvent(IGameEvent* event)
{
	if (!strcmp(event->getName(), XOR("player_footstep")))
	{
		esp::drawSound(event);
	}
	else if (!strcmp(event->getName(), XOR("player_death")))
	{
		// testing
		interfaces::console->consolePrintf("Died\n");
	}
	else if (!strcmp(event->getName(), XOR("game_start")))
	{
		globals::shotsFired = 0, globals::shotsHit = 0;
	}
	else if (!strcmp(event->getName(), XOR("player_hurt")))
	{
		misc::playHitmarker(event);
	}
	else if (!strcmp(event->getName(), XOR("weapon_fire")))
	{
		// here just check like that
		auto attacker = interfaces::entList->getClientEntity(interfaces::engine->getPlayerID(event->getInt(XOR("userid"))));
		if (!attacker)
			return;

		if (attacker == game::localPlayer)
			globals::shotsFired++;
	}
}

void Events::shutdown() const
{
	interfaces::eventManager->removeListener(&Events::g());
}