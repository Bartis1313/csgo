#pragma once

#include <classes/renderableToPresent.hpp>

#include <vector>

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