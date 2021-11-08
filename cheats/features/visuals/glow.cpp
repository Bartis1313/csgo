#include "glow.hpp"
#include "../../../SDK/interfaces/interfaces.hpp"
#include "../../../SDK/structs/Entity.hpp"
#include "../../game.hpp"
#include "../../menu/vars.hpp"

// TODO: what drops so hard???
void glow::run()
{
	if (!vars::bGlow)
		return;

	if (!game::localPlayer)
		return;

	for (int i = 0; i < interfaces::glowManager->m_size; i++)
	{
		auto& glow = interfaces::glowManager->m_objects[i];

		if (glow.unused())
			continue;

		auto ent = reinterpret_cast<Player_t*>(glow.m_entity);

		if (!ent)
			continue;

		if (!ent->isPlayer())
			continue;

		if (ent->isDormant() || !ent->isAlive())
			continue;
		
		if (ent->m_iTeamNum() != game::localPlayer->m_iTeamNum())
		{
			glow.set(Colors::Purple);
			glow.m_fullBloom = false;
		}
	}
}