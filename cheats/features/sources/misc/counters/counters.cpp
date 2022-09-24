#include "counters.hpp"

#include "../../events/events.hpp"

#include <SDK/IGameEvent.hpp>
#include <SDK/IClientEntityList.hpp>
#include <SDK/IVEngineClient.hpp>
#include <SDK/interfaces/interfaces.hpp>
#include <game/game.hpp>
#include <game/globals.hpp>
#include <utilities/tools/tools.hpp>

void Counters::init()
{
	g_Events.add(XOR("weapon_fire"), std::bind(&Counters::addShots, this, std::placeholders::_1));
	g_Events.add(XOR("player_hurt"), std::bind(&Counters::addHits, this, std::placeholders::_1));
	g_Events.add(XOR("round_start"), std::bind(&Counters::reset, this, std::placeholders::_1));
}

void Counters::addShots(IGameEvent* event)
{
	auto attacker = interfaces::entList->getClientEntity(interfaces::engine->getPlayerID(event->getInt(XOR("userid"))));
	if (!attacker)
		return;

	if (attacker == game::localPlayer)
		globals::shotsFired++;
}

void Counters::addHits(IGameEvent* event)
{
	auto attacker = interfaces::entList->getClientEntity(interfaces::engine->getPlayerID(event->getInt(XOR("attacker"))));
	if (!attacker)
		return;

	// very important
	if (attacker != game::localPlayer)
		return;

	auto ent = reinterpret_cast<Player_t*>(interfaces::entList->getClientEntity(interfaces::engine->getPlayerID(event->getInt(XOR("userid")))));
	if (!ent) // should never happen
		return;

	globals::shotsHit++;

	auto hitgroup = event->getInt(XOR("hitgroup"));

	if (hitgroup == 1) // HS
		globals::shotsHead++;
}

void Counters::reset(IGameEvent* event)
{
	globals::shotsFired = 0;
	globals::shotsHit = 0;
	globals::shotsHead = 0;
}