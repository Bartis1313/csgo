#include "bulletUpdater.hpp"

#include <SDK/Enums.hpp>
#include <SDK/clientHitVerify.hpp>
#include <gamememory/memory.hpp>
#include <cheats/game/game.hpp>

void BulletUpdater::run(int frame)
{
	if (frame != FRAME_START)
		return;

	if (!game::isAvailable())
		return;

	const auto bulletsList = game::localPlayer->m_vecBulletVerifyListClient();
	static int gameBulletCount = bulletsList.m_size; // init current count
	if (gameBulletCount == bulletsList.m_size)
	{
		m_lastBullets.clear();
		return;
	}

	for (int i = bulletsList.m_size; i > gameBulletCount; i--)
		m_lastBullets.emplace_back(bulletsList[i - 1].m_pos);

	
	if (bulletsList.m_size != gameBulletCount)
		gameBulletCount = bulletsList.m_size;
}

std::vector<Vec3> BulletUpdater::getLastBullets() const
{
	return m_lastBullets;
}