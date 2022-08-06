#pragma once

#include "../../../classes/createMove.hpp"
#include "../../../classes/overrideView.hpp"

#include "../../../SDK/math/Vector.hpp"

class CUserCmd;
class CViewSetup;
class FreeLookViewer;

// pubg like freelook
class FreeLook : public CreateMovePrePredictionType
{
public:
	FreeLook()
	{
		m_hacks.push_back(this);
		m_hacksRun.push_back(this);
	}

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
	FreeLookViewer()
	{
		m_hacks.push_back(this);
		m_hacksRun.push_back(this);
	}

	virtual void init();
	virtual void run(CViewSetup* view);
};

[[maybe_unused]] inline auto g_FreeLookView = FreeLookViewer{};
