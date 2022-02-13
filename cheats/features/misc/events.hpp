#pragma once
#include "../../../SDK/interfaces/interfaces.hpp"

// TODO: Add more events!
class Events : public IGameEventListener
{
public:
	void init() const;
	void shutdown() const;
protected:
	void FireGameEvent(IGameEvent* event) override;
} inline events;