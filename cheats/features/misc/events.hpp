#pragma once
#include "../../../SDK/interfaces/interfaces.hpp"
#include "../../../utilities/singleton.hpp"

// TODO: Add more events!
class Events : public IGameEventListener, public singleton<Events>
{
public:
	void init() const;
	void shutdown() const;
protected:
	void FireGameEvent(IGameEvent* event) override;
};