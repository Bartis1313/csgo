#pragma once

#include <classes/createMove.hpp>
#include <classes/overrideView.hpp>

#include <SDK/math/Vector.hpp>

class CUserCmd;
class CViewSetup;
class FreeLookViewer;

// pubg like freelook
class FreeLook : protected CreateMovePrePredictionType
{
public:
	constexpr FreeLook() :
		CreateMovePrePredictionType{}
	{}

protected:
	virtual void run(CUserCmd* cmd) override;
private:
	Vec3 m_lastAngle;
	Vec3 m_lastAngleLook;

	friend FreeLookViewer;
};

GLOBAL_FEATURE(FreeLook);

class FreeLookViewer : protected OverrideViewType
{
public:
	constexpr FreeLookViewer() :
		OverrideViewType{}
	{}

protected:
	virtual void run(CViewSetup* view) override;
};

GLOBAL_FEATURE(FreeLookViewer);
