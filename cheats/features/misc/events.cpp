#include "events.hpp"

#include "../../../SDK/IClientEntityList.hpp"
#include "../../../SDK/ICvar.hpp"
#include "../../../SDK/IVEngineClient.hpp"
#include "../../../SDK/CGlobalVars.hpp"
#include "../../../SDK/interfaces/interfaces.hpp"

#include "../visuals/player.hpp"
#include "../misc/misc.hpp"
#include "../visuals/world.hpp"

#include "../../game.hpp"
#include "../../globals.hpp"
#include "../../../utilities/console/console.hpp"
#include "../../../config/vars.hpp"

void Events::init() const
{
	interfaces::eventManager->addListener(&events, XOR("player_footstep"));
	interfaces::eventManager->addListener(&events, XOR("player_death"));
	interfaces::eventManager->addListener(&events, XOR("round_start"));
	interfaces::eventManager->addListener(&events, XOR("player_hurt"));
	interfaces::eventManager->addListener(&events, XOR("weapon_fire"));
	interfaces::eventManager->addListener(&events, XOR("bullet_impact"));
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
	else if (name == XOR("bullet_impact"))
	{
		auto attacker = interfaces::entList->getClientEntity(interfaces::engine->getPlayerID(event->getInt(XOR("userid"))));
		if (!attacker)
			return;

		if (attacker != game::localPlayer)
			return;

		auto local = reinterpret_cast<Player_t*>(attacker);

		float x = event->getFloat(XOR("x"));
		float y = event->getFloat(XOR("y"));
		float z = event->getFloat(XOR("z"));

		Vector src = local->getEyePos();
		Vector dst = { x, y, z };

		world.pushLocalImpacts({ src, dst, interfaces::globalVars->m_curtime + config.get<float>(vars.fDrawLocalSideImpacts) });
		misc.drawBulletTracer(src, dst);
	}
}

void Events::shutdown() const
{
	interfaces::eventManager->removeListener(&events);
}