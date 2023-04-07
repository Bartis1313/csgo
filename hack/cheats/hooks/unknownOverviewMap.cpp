#include "hooks.hpp"

#include "../classes/overViewMap.hpp"

#include <SDK/MapStruct.hpp>

hooks::unknownOverViewFun::value hooks::unknownOverViewFun::hooked(FAST_ARGS, int unk)
{
	const int ret = original(thisptr, unk);
	OverViewMapType::runAll(reinterpret_cast<MapStruct*>(thisptr));
	return ret;
}