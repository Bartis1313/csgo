#pragma once

#include <SDK/IGameEvent.hpp>
#include <classes/base.hpp>

#include <string>
#include <functional>
#include <vector>
#include <unordered_map>

// control init yourself
class Events : public IGameEventListener
{
public:
	Events()
	{
		m_allEvents.push_back(this);
	}

	void init();
	void shutdown();
	// place in virtual init() method of class using this
	void add(const std::string& eventName, const std::function<void(IGameEvent*)>& fun);
	static void shutdownAllEvents();
private:
	virtual void FireGameEvent(IGameEvent* event) override;

	inline static std::vector<Events*> m_allEvents;
	std::unordered_map<std::string, std::vector<std::function<void(IGameEvent*)>>> m_map;
};

GLOBAL_FEATURE(Events);
