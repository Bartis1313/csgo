#pragma once

#include <cheats/classes/renderableToPresent.hpp>

class PlayerList : protected RenderablePresentType
{
public:
	constexpr PlayerList() :
		RenderablePresentType{}
	{}
protected:
	virtual void draw() override;
};

GLOBAL_FEATURE(PlayerList);