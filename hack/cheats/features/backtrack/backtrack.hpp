#pragma once

#include <SDK/vars.hpp>
#include <SDK/math/Vector.hpp>
#include <SDK/math/matrix.hpp>

#include <deque>
#include <array>

class CUserCmd;
enum FrameStage;

namespace backtrack
{
	struct StoredRecord
	{
		float simtime{ };
		Vec3 head{ };
		Vec3 origin{ };
		Vec3 angle{ };
		std::array<Matrix3x4, BONE_USED_BY_HITBOX> matrices;
	};

	void init();
	void run(CUserCmd* cmd);

	[[nodiscard]] float getLerp();
	[[nodiscard]] bool isValid(float simtime);
	
	inline std::array<std::deque<StoredRecord>, 65> records;

	namespace updater
	{
		void run(FrameStage stage);
	}
};