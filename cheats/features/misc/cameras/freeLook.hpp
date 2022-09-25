#pragma once

#include <classes/createMove.hpp>
#include <classes/overrideView.hpp>

#include <SDK/math/Vector.hpp>

class CUserCmd;
class CViewSetup;
class FreeLookViewer;

// pubg like freelook
class FreeLook : public CreateMovePrePredictionType
{
public:
	constexpr FreeLook() :
		CreateMovePrePredictionType{}
	{}

	virtual void init();
	virtual void run(CUserCmd* cmd);
private:
	Vector m_lastAngle;
	Vector m_lastAngleLook;

	friend FreeLookViewer;
};

[[maybe_unused]] inline auto g_FreeLook = FreeLook{};

class FreeLookViewer : public OverrideViewType
{
public:
	constexpr FreeLookViewer() :
		OverrideViewType{}
	{}

	virtual void init();
	virtual void run(CViewSetup* view);
};

[[maybe_unused]] inline auto g_FreeLookView = FreeLookViewer{};
