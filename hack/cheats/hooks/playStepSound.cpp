#include "playStepSound.hpp"

#include <cheats/game/game.hpp>
#include <cheats/features/visuals/player/sounds.hpp>

hooks::PlayStepSound::value hooks::PlayStepSound::hook(FAST_ARGS, Vec3& origin, void* surface, float volume, bool force, void* unk)
{
	if (!thisptr)
		return original(thisptr, origin, surface, volume, force, unk);

	if(thisptr == game::localPlayer)
		return original(thisptr, origin, surface, volume, force, unk);

	sound::pushSteps(static_cast<Player_t*>(thisptr));

	return original(thisptr, origin, surface, volume, force, unk);
}