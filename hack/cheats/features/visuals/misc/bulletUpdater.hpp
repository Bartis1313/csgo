#pragma once

#include <SDK/math/Vector.hpp>

#include <vector>

enum FrameStage;

namespace bulletUpdater
{
	// 2 hit
	// 1 no hit
	[[nodiscard]] std::vector<Vec3> getLastBullets();

	void run(FrameStage stage);
}