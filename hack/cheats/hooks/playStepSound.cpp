#include "hooks.hpp"

#include <cheats/game/game.hpp>
#include <cheats/features/visuals/player/sounds.hpp>

hooks::playStepSound::value FASTCALL hooks::playStepSound::hooked(FAST_ARGS, Vec3& origin, void* surface, float volume, bool force, void* unk)
{
	if (!thisptr)
		return original(thisptr, origin, surface, volume, force, unk);

	if(thisptr == game::localPlayer)
		return original(thisptr, origin, surface, volume, force, unk);

	SoundDraw::pushSteps(reinterpret_cast<Player_t*>(thisptr));

	return original(thisptr, origin, surface, volume, force, unk);
}