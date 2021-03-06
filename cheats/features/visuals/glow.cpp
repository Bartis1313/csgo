#include "glow.hpp"

#include "../../../SDK/IMaterialSystem.hpp"
#include "../../../SDK/IVModelRender.hpp"
#include "../../../SDK/CGlowManager.hpp"
#include "../../../SDK/ClientClass.hpp"
#include "../../../SDK/Enums.hpp"
#include "../../../SDK/interfaces/interfaces.hpp"

#include "../../game.hpp"
#include "../../../config/vars.hpp"

void Glow::run()
{
	if (!config.get<bool>(vars.bGlow))
		return;

	if (!game::isAvailable())
		return;

	for (int i = 0; i < interfaces::glowManager->m_size; i++)
	{
		auto& glow = interfaces::glowManager->m_objects[i];

		if (glow.unused())
			continue;

		auto ent = reinterpret_cast<Player_t*>(glow.m_entity);

		if (!ent)
			continue;

		if (ent->isDormant())
			continue;
		
		auto cl = ent->clientClass();

		if (!cl)
			continue;

		switch (cl->m_classID)
		{
		case CCSPlayer:
		{
			if (ent->m_iTeamNum() != game::localPlayer->m_iTeamNum())
			{
				glow.set(config.get<CfgColor>(vars.cGlow).getColor());
				glow.m_fullBloom = false;
			}
			break;
		}
		default:
			break;
		}
	}
}