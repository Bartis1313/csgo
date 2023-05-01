#include "events.hpp"

#include <SDK/interfaces/interfaces.hpp>
#include <cheats/game/globals.hpp>

void EventCallback::addToCallback(const std::string_view eventName, const callbackType& callback)
{
	if (auto itr = events.find(eventName); itr != events.end())
	{
		// if found, then only push to vector with functions
		itr->second.push_back(callback);
	}
	else // create new
	{
		memory::interfaces::eventManager->addListener(this, eventName.data());
		events.emplace(std::make_pair
		(
			eventName,
			std::vector<callbackType>{ callback }
		));
	}
}

void EventCallback::fireGameEvent(IGameEvent* event)
{
	for (const auto [name, funcs] : events)
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