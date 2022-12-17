#include "hooks.hpp"

#include "../classes/overViewMap.hpp"

#include <SDK/MapStruct.hpp>

int FASTCALL hooks::unknownOverViewFun::hooked(FAST_ARGS, int unk)
{
	OverViewMapType::runAll(reinterpret_cast<MapStruct*>(thisptr));

	return original(thisptr, unk);
}