#pragma once

#include "../../../classes/onlyInit.hpp"

class IGameEvent;

// intended to only run it by engine
class BulletTracer : public OnlyInitType
{
public:
	BulletTracer()
	{
		m_hacks.push_back(this);
	}

	virtual void init();
private:
	void draw(IGameEvent* event);
};

[[maybe_unused]] inline auto g_BulletTracer = BulletTracer{};
