#pragma once

#include <features/classes/doPostScreen.hpp>

class Glow : public DoPostScreenType
{
public:
	constexpr Glow() :
		DoPostScreenType{}
	{}

	virtual void init();
	virtual void run();
};

[[maybe_unused]] inline auto g_Glow = Glow{};