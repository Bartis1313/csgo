#include "dispatchInnerParticlePrecip.hpp"

hooks::DispatchInnerParticlePrecip::value hooks::DispatchInnerParticlePrecip::hook(FAST_ARGS, int slot, void* player, Vec3 vec)
{
	original(thisptr, slot, player, vec);
}