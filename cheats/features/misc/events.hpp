#pragma once
#include "../../../SDK/interfaces/interfaces.hpp"
#include "../../../utilities/singleton.hpp"

// TODO: Add more events!
class Events : public IGameEventListener2, public singleton<Events>
{
public:
	void init();
	void shutdown();
	void FireGameEvent(IGameEvent* event);
};