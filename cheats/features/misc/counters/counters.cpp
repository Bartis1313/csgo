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
	events::add(XOR("weapon_fire"), std::bind(&Counters::addShots, this, std::placeholders::_1));
	events::add(XOR("player_hurt"), std::bind(&Counters::addHits, this, std::placeholders::_1));
	events::add(XOR("round_end"), std::bind(&Counters::resetShots, this, std::placeholders::_1));
}

void Counters::addShots(IGameEvent* event)
{
	auto attacker = memory::interfaces::entList->getClientEntity(memory::interfaces::engine->getPlayerID(event->getInt(XOR("userid"))));
	if (!attacker)
		return;

	if (attacker == game::localPlayer)
		globals::shotsFired++;
}

void Counters::addHits(IGameEvent* event)
{
	auto attacker = memory::interfaces::entList->getClientEntity(memory::interfaces::engine->getPlayerID(event->getInt(XOR("attacker"))));
	if (!attacker)
		return;

	// very important
	if (attacker != game::localPlayer)
		return;

	auto ent = reinterpret_cast<Player_t*>(memory::interfaces::entList->getClientEntity(memory::interfaces::engine->getPlayerID(event->getInt(XOR("userid")))));
	if (!ent) // should never happen
		return;

	globals::shotsHit++;

	auto hitgroup = event->getInt(XOR("hitgroup"));

	if (hitgroup == 1) // HS
		globals::shotsHead++;
}

void Counters::resetShots(IGameEvent* event)
{
	globals::shotsFired = 0;
	globals::shotsHit = 0;
	globals::shotsHead = 0;
}