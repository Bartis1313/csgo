#pragma once

#include "CUserCmd.hpp"
#include "math/Vector.hpp"

#include <utilities/pad.hpp>
#include <utilities/vfunc.hpp>

class Player_t;

class IMoveHelper
{
public:
	VFUNC(void, setHost, 1, (Player_t* host), (this, host));
};

class CMoveData
{
public:
	bool m_firstRunOfFunctions;
	bool m_gameCodeMovedPlayer;
	int m_playerHandle;
	int m_impulseCommand;
	Vec3 m_viewAngles;
	Vec3 m_absViewAngles;
	int m_buttons;
	int m_oldButtons;
	float m_forwardMove;
	float m_sideMove;
	float m_upMove;
	float m_maxSpeed;
	float m_clientMaxSpeed;
	Vec3 m_velocity;
	Vec3 m_angles;
	Vec3 m_oldAngles;
	float m_outStepHeight;
	Vec3 m_outWishVel;
	Vec3 m_outJumpVel;
	Vec3 m_constraintCenter;
	float m_constraintRadius;
	float m_constraintWidth;
	float m_constraintSpeedFactor;
	PAD(20);
	Vec3 m_absOrigin;
};

class IPrediction
{
public:
	VFUNC(void, update, 3, (int startframe, bool validframe, int incomingAcknowledged, int outgoingCommand),
		(this, startframe, validframe, incomingAcknowledged, outgoingCommand));
	VFUNC(void, setLocalViewangles, 13, (Vec3& angle), (this, std::ref(angle)));
	VFUNC(bool, inPrediction, 14, (), (this));
	VFUNC(void, setupMove, 20, (Player_t* localPlayer, CUserCmd* cmd, IMoveHelper* moveHelper, CMoveData* moveData), (this, localPlayer, cmd, moveHelper, moveData));
	VFUNC(void, finishMove, 21, (Player_t* localPlayer, CUserCmd* cmd, CMoveData* moveData), (this, localPlayer, cmd, moveData))
};
