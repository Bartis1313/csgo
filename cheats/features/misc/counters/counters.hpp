#pragma once

#include <classes/onlyInit.hpp>

class IGameEvent;

class Counters : protected OnlyInitType
{
public:
	constexpr Counters() :
		OnlyInitType{}
	{}

protected:
	virtual void init() override;
private:
	void addShots(IGameEvent* event);
	void addHits(IGameEvent* event);
	void resetShots(IGameEvent* event);
};

GLOBAL_FEATURE(Counters);