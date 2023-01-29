#pragma once

#include "helpers/vfunc.hpp"

class Player_t;
class CMoveData;

class CGameMovement
{
public:
	VFUNC(void, processMovement, 1, (Player_t* local, CMoveData* moveData), (this, local, moveData));
	VFUNC(void, reset, 2, (), (this));
	VFUNC(void, startTrackPredictionErrors, 3, (Player_t* local), (this, local));
	VFUNC(void, finishTrackPredictionErrors, 4, (Player_t* local), (this, local));
	VFUNC(void, decayAimPunchAngle, 77, (), (this));
};