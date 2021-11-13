#pragma once
#include "../../game.hpp"
#include "../../../SDK/interfaces/interfaces.hpp"
#include <deque>
#include <array>
// TODO: fix setupbones for better results

namespace backtrack
{
	struct StoredRecord
	{
		float simTime = 0.0f;
		Vector head = { 0, 0, 0 };
		// use origin to set abs or for whatever need
		Vector origin = { 0, 0 , 0 };
		matrix3x4_t matrix[BONE_USED_BY_HITBOX] = {};
	};

	void run(CUserCmd* cmd);
	void update();
	void init();
	bool isValid(float simtime);
	float getLerp();
	inline std::array<std::deque<StoredRecord>, 65> records;
};