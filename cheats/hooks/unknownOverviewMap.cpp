#include "hooks.hpp"

#include "../../SDK/MapStruct.hpp"
#include "../features/classes/overViewMap.hpp"

int __fastcall hooks::unknownOverViewFun::hooked(MapStruct* map, void* edx, int unk)
{
	OverViewMapType::runAll(map);

	return original(map, unk);
}