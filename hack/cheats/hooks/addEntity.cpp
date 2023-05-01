#include "hooks.hpp"

#include <SDK/IClientEntityList.hpp>
#include <SDK/structs/Entity.hpp>
#include <SDK/interfaces/interfaces.hpp>
#include <cheats/features/cache/cache.hpp>
#include "addEntity.hpp"

hooks::AddEntity::value hooks::AddEntity::hook(FAST_ARGS, void* handleEnt, EHandle_t handle)
{
	Entity_t* ent = reinterpret_cast<Entity_t*>(memory::interfaces::entList->getClientFromHandle(handle));
	
	EntityCache::add(ent);
	
	original(thisptr, handleEnt, handle);
}
