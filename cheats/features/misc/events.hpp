#pragma once

#include "../../../SDK/IGameEvent.hpp"

// TODO: Add more events!
class Events final : public IGameEventListener
{
public:
	void init() const;
	void shutdown() const;
protected:
	void FireGameEvent(IGameEvent* event) override;
};

inline Events events;