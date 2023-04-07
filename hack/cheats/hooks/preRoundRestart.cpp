#include "hooks.hpp"

#include <cheats/classes/base.hpp>
#include <SDK/IGameEvent.hpp>

// player_death, round_start, round_end
DEFINE_HOOK_CLASS_MEMBER(hooks::preRestartRound, void* event)
{
	const auto ev = reinterpret_cast<IGameEvent*>(event);
	const auto name = ev->getName();
	if (name == "round_end")
		BaseHack::restartRoundAll();

	original(thisptr, event);
}

#include <SDK/interfaces/interfaces.hpp>

#include <gamememory/memory.hpp>
#include <SDK/IVEngineClient.hpp>
#include <SDK/IClientEntityList.hpp>
#include <cheats/game/game.hpp>

#include <cheats/features/visuals/hitmarker/hitmark.hpp>
#include <cheats/features/visuals/misc/drawInfo.hpp>

// player_hurt
hooks::unkround::value FASTCALL hooks::unkround::hooked(FAST_ARGS, void* event)
{
	const auto ev = reinterpret_cast<IGameEvent*>(event);
	const auto name = ev->getName();

	const auto attacker = memory::interfaces::entList->getClientEntity(memory::interfaces::engine->getPlayerID(ev->getInt("attacker")));
	if (attacker == game::localPlayer)
	{
		g_Hitmarker->handleHits(ev);
		g_MiscInfo->addHits(ev);
	}

	original(thisptr, event);
}