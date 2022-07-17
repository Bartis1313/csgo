#pragma once

#include "../../../SDK/math/Vector.hpp"

class CUserCmd;
class CViewSetup;

// pubg like freelook
class FreeLook
{
public:
	void createMove(CUserCmd* cmd);
	void overrideView(CViewSetup* view);
private:
	Vector m_lastAngle;
	Vector m_lastAngleLook;
};

inline FreeLook freeLook;