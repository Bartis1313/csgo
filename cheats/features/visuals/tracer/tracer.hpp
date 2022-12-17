#pragma once

#include <classes/onlyInit.hpp>
#include <features/events/events.hpp>

class IGameEvent;

// intended to only run it by engine
class BulletTracer : protected OnlyInitType
{
public:
	constexpr BulletTracer() :
		OnlyInitType{}
	{}

protected:
	virtual void init() override;
private:
	void draw(IGameEvent* event);
};

GLOBAL_FEATURE(BulletTracer);
