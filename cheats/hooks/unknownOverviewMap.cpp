#include "hooks.hpp"

#include "../../SDK/MapStruct.hpp"
#include "../features/visuals/radar.hpp"
#include "../features/visuals/world.hpp"

int __fastcall hooks::unknownOverViewFun::hooked(MapStruct* map, void* edx, int unk)
{
	radar.onInit(map);
	// manually so on new map = new ent
	world.setWeatherState(false);

	return original(map, unk);
}