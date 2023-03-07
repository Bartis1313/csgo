#pragma once

#include <cheats/classes/overrideView.hpp>
#include <cheats/classes/wndProcKeyHandler.hpp>

class CViewSetup;

class Thirdperson : protected OverrideViewType, protected WndProcKeyHandler
{
public:
	constexpr Thirdperson() :
		OverrideViewType{},
		WndProcKeyHandler{}
	{}

protected:
	virtual void run([[maybe_unused]] CViewSetup* view) override;
	virtual void updateKeys() override;
};

GLOBAL_FEATURE(Thirdperson);
