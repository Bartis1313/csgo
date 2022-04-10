#include "events.hpp"

#include "../../../SDK/IClientEntityList.hpp"
#include "../../../SDK/ICvar.hpp"
#include "../../../SDK/IVEngineClient.hpp"

#include "../../../SDK/interfaces/interfaces.hpp"

#include "../visuals/player.hpp"
#include "../misc/misc.hpp"

#include "../../game.hpp"
#include "../../globals.hpp"
#include "../../../utilities/console/console.hpp"


void Events::init() const
{
	interfaces::eventManager->addListener(&events, XOR("player_footstep"));
	interfaces::eventManager->addListener(&events, XOR("player_death"));
	interfaces::eventManager->addListener(&events, XOR("round_start"));
	interfaces::eventManager->addListener(&events, XOR("player_hurt"));
	interfaces::eventManager->addListener(&events, XOR("weapon_fire"));
	console.log(TypeLogs::LOG_INFO, XOR("events init"));
}

void Events::FireGameEvent(IGameEvent* event)
{
	if (std::string_view name = event->getName(); name == XOR("player_footstep"))
	{
		visuals.drawSound(event);
	}
	else if (name == XOR("player_death"))
	{
		// testing
		interfaces::cvar->consolePrintf("Died\n");
	}
	else if (name == XOR("game_start"))
	{
		globals::shotsFired = 0, globals::shotsHit = 0, globals::shotsHead = 0;
	}
	else if (name == XOR("player_hurt"))
	{
		misc.playHitmarker(event);
	}
	else if (name == XOR("weapon_fire"))
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
	interfaces::eventManager->removeListener(&events);
}