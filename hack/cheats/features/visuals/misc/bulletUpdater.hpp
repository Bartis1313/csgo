#pragma once

#include <cheats/classes/frameStage.hpp>
#include <SDK/math/Vector.hpp>

#include <vector>

class BulletUpdater : protected FrameStageType
{
public:
	constexpr BulletUpdater() :
		FrameStageType{}
	{}
	// 2 hit
	// 1 no hit
	[[nodiscard]] std::vector<Vec3> getLastBullets() const;
protected:
	virtual void run(int frame) override;
private:
	std::vector<Vec3> m_lastBullets;
};

GLOBAL_FEATURE(BulletUpdater);