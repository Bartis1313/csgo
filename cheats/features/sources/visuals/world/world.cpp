#include "world.hpp"

#include "../../../../../SDK/IClientEntityList.hpp"
#include "../../../../../SDK/ClientClass.hpp"
#include "../../../../../SDK/interfaces/interfaces.hpp"
#include "../../../../../SDK/structs/Entity.hpp"

#include "bomb.hpp"
#include "fog.hpp"
#include "molotov.hpp"
#include "smoke.hpp"
#include "tone.hpp"
#include "projectiles.hpp"
#include "nadewarn.hpp"


void World::init()
{

}

void World::draw()
{
	const auto maxIndex = interfaces::entList->getHighestIndex();
	for (int i = 1; i <= maxIndex; i++)
	{
		auto entity = reinterpret_cast<Entity_t*>(interfaces::entList->getClientEntity(i));

		if (!entity)
			continue;

		if (entity->isDormant())
			continue;

		if (entity->isPlayer())
			continue;

		const auto cl = entity->clientClass();

		if (!cl)
			continue;

		g_Projectiles.run(entity, cl->m_classID);
		g_GrenadeWarning.run(reinterpret_cast<Nade_t*>(entity));

		switch (cl->m_classID)
		{
		case CInferno:
			g_Molotov.draw(reinterpret_cast<Inferno_t*>(entity));
			break;
		case CPlantedC4:
			g_BombOverlay.setBombEnt(reinterpret_cast<Bomb_t*>(entity));
			break;
		case CSmokeGrenadeProjectile:
			g_SmokeSraw.draw(reinterpret_cast<Smoke_t*>(entity));
			break;
		case CFogController:
			g_FogController.run(reinterpret_cast<FogController_t*>(entity));
			break;
		case CEnvTonemapController:
			g_ToneController.run(reinterpret_cast<EnvTonemapController_t*>(entity));
			break;
		default:
			break;
		}
	}
}
