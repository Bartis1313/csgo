#include "address.hpp"

#include <SDK/interfaces/interfaces.hpp>
#include <SDK/IBaseClientDll.hpp>
#include <SDK/structs/Entity.hpp>
#include <SDK/IBaseClientDll.hpp>

memory::Address<uintptr_t> memory::findFromGame(ClassID id)
{
	for (auto _class = memory::interfaces::client->getAllClasses(); _class; _class = _class->m_next)
	{
		if (_class->m_classID == id)
			return Address<uintptr_t>{ reinterpret_cast<uintptr_t>(_class) };
	}

	return Address<uintptr_t>{ 0U };
}

#include <SDK/IVEngineClient.hpp>
#include <SDK/IClientEntityList.hpp>

memory::Address<uintptr_t> memory::findFromGameLoop(ClassID id)
{
	for (auto i = memory::interfaces::engine->getMaxClients(); i <= memory::interfaces::entList->getHighestIndex(); i++)
	{
		auto entity = reinterpret_cast<Entity_t*>(memory::interfaces::entList->getClientEntity(i));
		if (!entity)
			continue;

		auto idx = entity->clientClass()->m_classID;
		if (idx == id)
			return Address<uintptr_t>{ reinterpret_cast<uintptr_t>(entity) };
	}

	return Address<uintptr_t>{ 0U };
}
