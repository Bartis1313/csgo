#pragma once

#include <cheats/classes/frameStage.hpp>
#include <cheats/classes/createMove.hpp>
#include <SDK/vars.hpp>
#include <SDK/math/Vector.hpp>
#include <SDK/math/matrix.hpp>

#include <deque>
#include <array>

class CUserCmd;
class INetChannel;
class IConVar;
class BackTrackUpdater;

class Backtrack : protected CreateMoveInPredictionType
{
public:
	Backtrack() :
		CreateMoveInPredictionType{}
	{}

	[[nodiscard]] bool isValid(float simtime) const;
protected:
	virtual void run(CUserCmd* cmd);
	virtual void init();
private:
	[[nodiscard]] float getLerp() const;
	[[nodiscard]] float extraTicks() const;

	struct StoredRecord
	{
		float m_simtime = 0.0f;
		Vec3 m_head = {};
		// use origin to set abs or for whatever need
		Vec3 m_origin = {};
		std::array<Matrix3x4, BONE_USED_BY_HITBOX> m_matrix;
	};

	struct convars
	{
		IConVar* updateRate = nullptr;
		IConVar* maxUpdateRate = nullptr;
	} cvars;

	struct convarRatios
	{
		float interp;
		float interpRatio;
		float minInterpRatio;
		float maxInterpRatio;
		float maxUnlag;
	} cvarsRatios;
	
	std::array<std::deque<StoredRecord>, 65> m_records;
public:
	[[nodiscard]] auto& getAllRecords() { return m_records; }

	friend BackTrackUpdater;
};

GLOBAL_FEATURE(Backtrack);

class BackTrackUpdater : protected FrameStageType
{
public:
	constexpr BackTrackUpdater() :
		FrameStageType{}
	{}

protected:
	virtual void run(int frame) override;
private:
	float m_correctTime;
};

GLOBAL_FEATURE(BackTrackUpdater);
