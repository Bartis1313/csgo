#pragma once

#include <classes/onlyInit.hpp>

class IGameEvent;

class Counters : public OnlyInitType
{
public:
	constexpr Counters() :
		OnlyInitType{}
	{}

	virtual void init();
private:
	void addShots(IGameEvent* event);
	void addHits(IGameEvent* event);
	void reset(IGameEvent* event);
};

[[maybe_unused]] inline auto g_Counters = Counters{};