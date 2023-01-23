#include "events.hpp"

#include <SDK/interfaces/interfaces.hpp>
#include <cheats/game/globals.hpp>

void EventCallback::addToCallback(const std::string_view eventName, const std::function<void(IGameEvent*)>& callback)
{
	if (auto itr = m_map.find(eventName); itr != m_map.end())
	{
		// if found, then only push to vector with functions
		itr->second.push_back(callback);
	}
	else // create new
	{
		memory::interfaces::eventManager->addListener(this, eventName.data());
		m_map.emplace(std::make_pair
		(
			eventName,
			std::vector<std::function<void(IGameEvent*)>>{ callback }
		));
	}
}

void EventCallback::FireGameEvent(IGameEvent* event)
{
	for (const auto [name, funcs] : m_map)
	{
		if (const std::string_view ename{ event->getName() }; ename == name)
		{
			for (const auto& func : funcs)
				func(event);
		}
	}
}

void EventCallback::shutdown()
{
	memory::interfaces::eventManager->removeListener(this);
}