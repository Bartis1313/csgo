#include "removeEntity.hpp"

#include <cheats/features/cache/cache.hpp>
#include <gamememory/memory.hpp>
#include <SDK/IClientEntityList.hpp>
#include <SDK/interfaces/interfaces.hpp>

hooks::RemoveEntity::value hooks::RemoveEntity::hook(FAST_ARGS, void* handleEnt, CBaseHandle handle)
{
	Entity_t* ent = reinterpret_cast<Entity_t*>(memory::interfaces::entList->getClientFromHandle(handle));
	EntityCache::erase(ent);

	original(thisptr, handleEnt, handle);
}