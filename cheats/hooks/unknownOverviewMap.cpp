#include "hooks.hpp"

#include "../classes/overViewMap.hpp"

#include <SDK/MapStruct.hpp>

int __fastcall hooks::unknownOverViewFun::hooked(MapStruct* map, void* edx, int unk)
{
	OverViewMapType::runAll(map);

	return original(map, unk);
}