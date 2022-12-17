#pragma once

#include <SDK/IGameEvent.hpp>

#include <string>
#include <functional>
#include <vector>
#include <unordered_map>

class EventsWrapper : public IGameEventListener
{
public:
	constexpr std::string_view getName() const { return m_name; }
protected:
	std::string_view m_name;
	virtual void FireGameEvent(IGameEvent* event) override {};
};

class EventCallback : public EventsWrapper
{
public:
	void addToCallback(const std::string_view eventName, const std::function<void(IGameEvent*)>& callback);
	void shutdown();
private:
	virtual void FireGameEvent(IGameEvent* event) override;
	std::unordered_map<std::string_view, std::vector<std::function<void(IGameEvent*)>>> m_map;
};

namespace events
{
	inline EventCallback globalEvent;
	inline void shutdown()
	{
		globalEvent.shutdown();
	}
	inline void add(const std::string_view eventName, const std::function<void(IGameEvent*)>& callback)
	{
		globalEvent.addToCallback(eventName, callback);
	}
}
