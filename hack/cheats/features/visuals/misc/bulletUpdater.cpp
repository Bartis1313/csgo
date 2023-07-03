#include "bulletUpdater.hpp"

#include <SDK/Enums.hpp>
#include <SDK/clientHitVerify.hpp>
#include <SDK/CUtlVector.hpp>
#include <gamememory/memory.hpp>
#include <cheats/game/game.hpp>

#include <cheats/hooks/frameStageNotify.hpp>

namespace
{
	struct BulletHandler : hooks::FrameStageNotify
	{
		BulletHandler()
		{
			this->registerRun(bulletUpdater::run);
		}
	} bulletHandler;
}

namespace bulletUpdater
{
	std::vector<Vec3> lastBullets;
}

void bulletUpdater::run(FrameStage stage)
{
	if (stage != FRAME_START)
		return;

	if (!game::isAvailable())
		return;

	const auto bulletsList = game::localPlayer->m_vecBulletVerifyListClient();
	static int gameBulletCount = bulletsList.m_size; // init current count
	if (gameBulletCount == bulletsList.m_size)
	{
		lastBullets.clear();
		return;
	}

	for (int i = bulletsList.m_size; i > gameBulletCount; i--)
		lastBullets.emplace_back(bulletsList[i - 1].m_pos);
	
	if (bulletsList.m_size != gameBulletCount)
		gameBulletCount = bulletsList.m_size;
}

std::vector<Vec3> bulletUpdater::getLastBullets()
{
	return lastBullets;
}