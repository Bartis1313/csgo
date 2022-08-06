#pragma once

#include "../../../classes/doPostScreen.hpp"

class Thirdperson : public DoPostScreenType
{
public:
	constexpr Thirdperson() :
		DoPostScreenType{}
	{}

	virtual void init();
	virtual void run();
};

[[maybe_unused]] inline auto g_Thirdperson = Thirdperson{};
