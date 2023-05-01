#pragma once

#include <SDK/IGameEvent.hpp>

#include <string>
#include <functional>
#include <vector>
#include <unordered_map>

class EventsWrapper : public IGameEventListener
{
public:
	constexpr std::string_view getName() const { return name; }
protected:
	std::string_view name{ };
	virtual void fireGameEvent(IGameEvent* event) override {};
};

class EventCallback : public EventsWrapper
{
public:
	using callbackType = std::function<void(IGameEvent*)>;

	void addToCallback(const std::string_view eventName, const callbackType& callback);
	void shutdown();
private:
	virtual void fireGameEvent(IGameEvent* event) override;
	std::unordered_map<std::string_view, std::vector<callbackType>> events{ };
};

namespace events
{
	inline EventCallback globalEvent{ };
	inline void shutdown()
	{
		globalEvent.shutdown();
	}
	inline void add(const std::string_view eventName, const EventCallback::callbackType& callback)
	{
		globalEvent.addToCallback(eventName, callback);
	}
}
