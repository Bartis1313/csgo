#pragma once

#include <cheats/classes/onlyInit.hpp>
#include <SDK/IPrediction.hpp>

#include <functional>

class CUserCmd;
class Player_t;
class Weapon_t;

class Prediction : protected OnlyInitType
{
public:
	constexpr Prediction() :
		OnlyInitType{}
	{}

	void addToPrediction(CUserCmd* cmd, const std::function<void()>& fun);
	void update();
protected:
	virtual void init() override;
private:
	struct PredictionCache
	{
		bool isInPrediction;
		bool isFirstTimePredicted;
		float curTime;
		float frameTime;
		int tick;
		float weaponPenalty;
		float recoilIndex;
	} cache;

	void start(CUserCmd* cmd);
	void end();
	void patchDatamap();
	float m_curTime;
	float m_frameTime;
	uintptr_t* m_predicionRandomSeed;
	CMoveData* m_data;
	Player_t** m_player;
	std::unique_ptr<byte[]> m_startData;
	std::unique_ptr<byte[]> m_endData;
	size_t m_allocSize;
};

GLOBAL_FEATURE(Prediction);
