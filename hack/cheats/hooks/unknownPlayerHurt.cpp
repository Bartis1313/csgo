#include "unknownPlayerHurt.hpp"

#include <SDK/IVEngineClient.hpp>
#include <SDK/IClientEntityList.hpp>
#include <SDK/interfaces/interfaces.hpp>

#include <gamememory/memory.hpp>
#include <cheats/game/game.hpp>

hooks::UnknownPlayerHurt::value hooks::UnknownPlayerHurt::hook(FAST_ARGS, void* _event)
{
	const auto ev = reinterpret_cast<IGameEvent*>(_event);
	const auto name = ev->getName();

	const auto attacker = memory::interfaces::entList->getClientEntity(memory::interfaces::engine->getPlayerID(ev->getInt("attacker")));
	if (attacker == game::localPlayer)
	{
		Storage::runs.run(ev);
	}

	original(thisptr, _event);
}