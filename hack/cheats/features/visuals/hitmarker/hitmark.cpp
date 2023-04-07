#include "hitmark.hpp"

#include <SDK/CGlobalVars.hpp>
#include <SDK/IGameEvent.hpp>
#include <SDK/IVEngineClient.hpp>
#include <SDK/IClientEntityList.hpp>
#include <SDK/ISurface.hpp>
#include <SDK/interfaces/interfaces.hpp>
#include <SDK/clientHitVerify.hpp>
#include <gamememory/memory.hpp>
#include <cheats/game/game.hpp>
#include <cheats/game/globals.hpp>
#include <config/vars.hpp>
#include <render/render.hpp>
#include <render/BBox.hpp>
#include <utilities/tools/tools.hpp>
#include <utilities/tools/wrappers.hpp>
#include <cheats/features/events/events.hpp>

#include <functional>
#include <imgui_internal.h>

#include "../player/boxes.hpp"

void Hitmarker::draw()
{
	if (!vars::misc->hitmarker->enabled)
		return;

	if (!game::localPlayer)
		return;

	if (!game::localPlayer->isAlive())
		return;

	float x, y;
	const bool mode3D = vars::misc->hitmarker->enabled3D;
	if (!mode3D)
	{
		x = globals::screenX / 2.0f;
		y = globals::screenY / 2.0f;
	}

	const auto bulletList = game::localPlayer->m_vecBulletVerifyListClient();

	for (size_t i = 0; auto & [expire, damage, health, wasHeadhsot, player, hitPos, alpha] : m_hitmarkers)
	{
		// cant think of anything else, hitgroups are limited
		if (const auto bulletPos = bulletList[bulletList.m_size - 2].m_pos; bulletPos != hitPos)
			hitPos = bulletPos;

		const float diff = expire - memory::interfaces::globalVars->m_curtime;
		if (diff <= 0.0f)
		{
			m_hitmarkers.erase(m_hitmarkers.begin() + i);
			continue;
		}

		if (mode3D)
		{
			if (ImVec2 s; imRender.worldToScreen(hitPos, s))
			{
				x = s.x;
				y = s.y;
			}
		}

		alpha = diff / vars::misc->hitmarker->time;
		Color actualColor = vars::misc->hitmarker->colorNormal().getColorEditAlpha(alpha);
		float lineX = 10.0f;
		float lineY = 5.0f;

		if (health >= 0 && wasHeadhsot)
		{
			actualColor = vars::misc->hitmarker->colorHead().getColorEditAlpha(alpha);
			lineX = 14.0f;
			lineY = 7.0f;
		}
		else if (health <= 0)
		{
			actualColor = vars::misc->hitmarker->colorDead().getColorEditAlpha(alpha);
			lineX = 18.0f;
			lineY = 9.0f;
		}

		float lineAddonX = lineX;
		float lineAddonY = lineY;
		if (vars::misc->hitmarker->enabledResize)
		{
			lineAddonX = lineX * alpha;
			lineAddonY = lineY * alpha;
		}

		imRender.drawLine(x - lineAddonX, y + lineAddonX, x - lineAddonY, y + lineAddonY, actualColor);
		imRender.drawLine(x + lineAddonX, y + lineAddonX, x + lineAddonY, y + lineAddonY, actualColor);
		imRender.drawLine(x - lineAddonX, y - lineAddonX, x - lineAddonY, y - lineAddonY, actualColor);
		imRender.drawLine(x + lineAddonX, y - lineAddonX, x + lineAddonY, y - lineAddonY, actualColor);
		
		Box box{ player };
		if (!box.isValid())
			continue;

		const auto headPos = player->getHitboxPos(HITBOX_HEAD);

		const auto text = std::format("{}", damage);
		const auto textSize = imRender.getTextSize(ImFonts::franklinGothic30, text);
		const float fontSize = game::getScaledFont(game::localPlayer->getEyePos(), headPos, 150.0f, 12.0f, 30.0f);

		ImVec2 coord{ (box.x + box.w / 2) - textSize.x / 2, box.y };
		coord.y -= (i + 1) * fontSize; // some scaling with alpha would be nice
		imRender.text(coord.x, coord.y, fontSize, ImFonts::franklinGothic30, text, true, actualColor, false);

		i++;
	}
}

void Hitmarker::handleHits(IGameEvent* event)
{
	if (!vars::misc->hitmarker->enabled)
		return;

	const auto attacker = memory::interfaces::entList->getClientEntity(memory::interfaces::engine->getPlayerID(event->getInt("attacker")));
	if (!attacker)
		return;

	// very important
	if (attacker != game::localPlayer)
		return;

	const auto ent = reinterpret_cast<Player_t*>(memory::interfaces::entList->getClientEntity(memory::interfaces::engine->getPlayerID(event->getInt("userid"))));
	if (!ent) // should never happen
		return;

	const auto dmg_health = event->getInt("dmg_health");
	const auto health = ent->m_iHealth() - dmg_health;
	const auto hitgroup = event->getInt("hitgroup");

	Hitmark_t hit // rest fields are controlled in loop
	{
		.expireTime = memory::interfaces::globalVars->m_curtime + vars::misc->hitmarker->time,
		.damage = dmg_health,
		.health = ent->m_iHealth() - dmg_health,
		.wasHeadshot = hitgroup == HITGROUP_HEAD,
		.player = ent
	};

	m_hitmarkers.push_back(hit);

	if (vars::misc->hitmarker->play)
		memory::interfaces::surface->playSound("buttons\\arena_switch_press_02.wav");
}