#include "hooks.hpp"

#include "../../SDK/MapStruct.hpp"
#include "../features/visuals/radar.hpp"

int __fastcall hooks::unknownOverViewFun::hooked(MapStruct* map, void* edx, int unk)
{
	radar.onInit(map);

	return original(map, unk);
}