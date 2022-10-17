#pragma once

#include <classes/overrideView.hpp>
#include <SDK/math/Vector.hpp>

class CViewSetup;

class Freecam : protected OverrideViewType
{
public:
	constexpr Freecam() :
		OverrideViewType{}
	{}

	[[nodiscard]] constexpr bool isInCam() const { return m_inCam; }
protected:
	// need to set angles to remove view effect
	virtual void run(CViewSetup* view) override;
private:
	bool m_inCam = false;
	Vec3 m_view;
};

GLOBAL_FEATURE(Freecam);

#include <classes/renderableToPresent.hpp>

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