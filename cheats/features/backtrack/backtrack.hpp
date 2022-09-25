#pragma once

#include <classes/frameStage.hpp>
#include <classes/createMove.hpp>
#include <SDK/vars.hpp>
#include <SDK/math/Vector.hpp>
#include <SDK/math/matrix.hpp>

#include <deque>
#include <array>

class CUserCmd;
class INetChannel;
class IConVar;
class BackTrackUpdater;

class Backtrack : public CreateMoveInPredictionType
{
public:
	Backtrack() :
		CreateMoveInPredictionType{}
	{}

	virtual void run(CUserCmd* cmd);
	virtual void init();
	_NODISCARD bool isValid(float simtime) const;
private:
	_NODISCARD float getLerp() const;
	_NODISCARD float extraTicks() const;

	struct StoredRecord
	{
		float m_simtime = 0.0f;
		Vector m_head = {};
		// use origin to set abs or for whatever need
		Vector m_origin = {};
		std::array<Matrix3x4, BONE_USED_BY_HITBOX> m_matrix;
	};

	struct convars
	{
		IConVar* updateRate;
		IConVar* maxUpdateRate;
		IConVar* minUpdateRate;
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
	_NODISCARD auto& getAllRecords() { return m_records; }

	friend BackTrackUpdater;
};

[[maybe_unused]] inline auto g_Backtrack = Backtrack{};

class BackTrackUpdater : public FrameStageType
{
public:
	constexpr BackTrackUpdater() :
		FrameStageType{}
	{}

	virtual void init();
	virtual void run(int frame);
};

[[maybe_unused]] inline auto g_BackTrackUpdater = BackTrackUpdater{};
