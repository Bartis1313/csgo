#include "hooks.hpp"

#include <SDK/IClientEntityList.hpp>
#include <SDK/structs/Entity.hpp>
#include <SDK/interfaces/interfaces.hpp>
#include <cheats/features/cache/cache.hpp>

hooks::removeEnt::value FASTCALL hooks::removeEnt::hooked(FAST_ARGS, void* handleEnt, EHandle_t handle)
{
	Entity_t* ent = reinterpret_cast<Entity_t*>(memory::interfaces::entList->getClientFromHandle(handle));
	EntityCache::erase(ent);

	original(thisptr, handleEnt, handle);
}