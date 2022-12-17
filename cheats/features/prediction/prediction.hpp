#pragma once

#include <classes/onlyInit.hpp>
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

	int& getCorrectTick(CUserCmd* cmd = nullptr);
	void start(CUserCmd* cmd);
	void end();
	float m_curTime;
	float m_frameTime;
	uintptr_t* m_predicionRandomSeed;
	CMoveData* m_data;
	Player_t** m_player;
};

GLOBAL_FEATURE(Prediction);
