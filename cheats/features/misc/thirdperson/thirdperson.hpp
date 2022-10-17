#pragma once

#include <classes/overrideView.hpp>

class CViewSetup;

class Thirdperson : protected OverrideViewType
{
public:
	constexpr Thirdperson() :
		OverrideViewType{}
	{}

protected:
	virtual void run([[maybe_unused]] CViewSetup* view) override;
};

GLOBAL_FEATURE(Thirdperson);
