#include "processSpottedEntityUpdate.hpp"

#include <cheats/features/cache/cache.hpp>

hooks::ProcessSpottedEntityUpdate::value hooks::ProcessSpottedEntityUpdate::hook(FAST_ARGS, void* unk) // CCSUsrMsg_ProcessSpottedEntityUpdate
{
	//int size = *reinterpret_cast<int*>((uintptr_t)unk + 0xC);
	//printf("size is %i\n", size);

	CacheFields::update();

	return original(thisptr, unk);
}