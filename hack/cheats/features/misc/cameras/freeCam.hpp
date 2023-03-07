#pragma once

#include <cheats/classes/overrideView.hpp>
#include <cheats/classes/wndProcKeyHandler.hpp>
#include <SDK/math/Vector.hpp>

class CViewSetup;

class Freecam : protected OverrideViewType, protected WndProcKeyHandler
{
public:
	constexpr Freecam() :
		OverrideViewType{},
		WndProcKeyHandler{}
	{}

	[[nodiscard]] constexpr bool isInCam() const { return m_inCam; }
protected:
	// need to set angles to remove view effect
	virtual void run(CViewSetup* view) override;
	virtual void updateKeys() override;
private:
	bool m_inCam = false;
	Vec3 m_view;
};

GLOBAL_FEATURE(Freecam);

#include <cheats/classes/renderableToPresent.hpp>

class FreecamDraw : protected RenderablePresentType
{
public:
	constexpr FreecamDraw() :
		RenderablePresentType{}
	{}

protected:
	virtual void draw() override;
};

GLOBAL_FEATURE(FreecamDraw);