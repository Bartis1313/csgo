#pragma once

#include <classes/doPostScreen.hpp>

class Glow : protected DoPostScreenType
{
public:
	constexpr Glow() :
		DoPostScreenType{}
	{}

protected:
	virtual void run() override;
};

GLOBAL_FEATURE(Glow);