#pragma once

#include "../../../SDK/IGameEvent.hpp"

#include <unordered_map>
#include <string>
#include <functional>

class Events;
struct StructEvent_t
{
	Events* m_event;
	std::string_view m_name;
	std::function<void(IGameEvent*)> m_fun;
};

class Events final : public IGameEventListener
{
public:
	void init();
	void shutdown() const;
private:
	void FireGameEvent(IGameEvent* event) override;
	void add(const std::string_view eventName, const std::function<void(IGameEvent*)>& fun);
	std::vector<StructEvent_t> m_events;
};

inline Events events;