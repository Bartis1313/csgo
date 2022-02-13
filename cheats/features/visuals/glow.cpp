#include "glow.hpp"
#include "../../../SDK/interfaces/interfaces.hpp"
#include "../../game.hpp"
#include "../../../config/vars.hpp"

void glow::run()
{
	if (!config.get<bool>(vars.bGlow))
		return;

	if (!game::localPlayer)
		return;

	auto material = interfaces::matSys->findMaterial(XOR("dev/glow_color"), TEXTURE_GROUP_OTHER, true);
	interfaces::modelRender->overrideMaterial(material);


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
				glow.set(Colors::Purple);
				glow.m_fullBloom = false;
			}
			break;
		}
		default:
			break;
		}
	}
}