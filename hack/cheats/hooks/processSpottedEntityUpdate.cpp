#include "processSpottedEntityUpdate.hpp"

#include <cheats/features/cache/cache.hpp>

hooks::ProcessSpottedEntityUpdate::value hooks::ProcessSpottedEntityUpdate::hook(HACK_FAST_ARGS, void* unk) // CCSUsrMsg_ProcessSpottedEntityUpdate
{
	//int size = *reinterpret_cast<int*>((uintptr_t)unk + 0xC);
	//printf("size is %i\n", size);

	EntityCache::CacheFields::update();

	return original(thisptr, unk);
}