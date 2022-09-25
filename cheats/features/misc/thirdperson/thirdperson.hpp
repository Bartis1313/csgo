#pragma once

#include <classes/overrideView.hpp>

class CViewSetup;

class Thirdperson : public OverrideViewType
{
public:
	constexpr Thirdperson() :
		OverrideViewType{}
	{}

	virtual void init();
	virtual void run([[maybe_unused]] CViewSetup* view);
};

[[maybe_unused]] inline auto g_Thirdperson = Thirdperson{};
