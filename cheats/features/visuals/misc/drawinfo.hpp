#pragma once

#include <classes/renderableToPresent.hpp>

class MiscInfo : protected RenderablePresentType
{
public:
	constexpr MiscInfo() :
		RenderablePresentType{}
	{}

protected:
	virtual void draw() override;
};

GLOBAL_FEATURE(MiscInfo);