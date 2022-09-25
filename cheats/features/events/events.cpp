#include "events.hpp"

#include <SDK/interfaces/interfaces.hpp>

void Events::add(const std::string& eventName, const std::function<void(IGameEvent*)>& fun)
{
	if (auto itr = m_map.find(eventName); itr != m_map.end())
	{
		// if found, then only push to vector with functions
		itr->second.push_back(fun);
	}
	else // create new
	{
		m_map.emplace(std::make_pair
		(
			eventName,
			std::vector<std::function<void(IGameEvent*)>>{ fun }
		));
	}
}

void Events::init()
{
	// go through all map elements with making sure that there is always unique event name
	for (const auto& [name, functions] : m_map)
		interfaces::eventManager->addListener(this, name.c_str());
}

void Events::FireGameEvent(IGameEvent* event)
{
	if (auto itr = m_map.find(event->getName()); itr != m_map.end())
	{
		// if name is found go through all funcs
		for (const auto& el : itr->second)
			el(event);
	}
}

void Events::shutdown()
{
	interfaces::eventManager->removeListener(this);
}

void Events::shutdownAllEvents()
{
	for (auto el : m_allEvents)
		el->shutdown();
}
