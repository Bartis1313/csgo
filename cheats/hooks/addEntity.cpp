#include "hooks.hpp"

#include <SDK/IClientEntityList.hpp>
#include <SDK/structs/Entity.hpp>
#include <SDK/interfaces/interfaces.hpp>
#include <features/cache/cache.hpp>

void FASTCALL hooks::addEnt::hooked(FAST_ARGS, void* handleEnt, int handle)
{
	Entity_t* ent = reinterpret_cast<Entity_t*>(memory::interfaces::entList->getClientFromHandle(handle));
	EntityCache::add(ent);

	original(thisptr, handleEnt, handle);
}