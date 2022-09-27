#include "hitmark.hpp"

#include <SDK/CGlobalVars.hpp>
#include <SDK/IGameEvent.hpp>
#include <SDK/IVEngineClient.hpp>
#include <SDK/IClientEntityList.hpp>
#include <SDK/ISurface.hpp>
#include <SDK/interfaces/interfaces.hpp>
#include <game/game.hpp>
#include <game/globals.hpp>
#include <config/vars.hpp>
#include <utilities/renderer/renderer.hpp>
#include <utilities/tools/tools.hpp>
#include <utilities/tools/wrappers.hpp>
#include <cheats/features/events/events.hpp>

#include <functional>

void Hitmarker::init()
{
	g_Events.add(XOR("player_hurt"), std::bind(&Hitmarker::handleHits, this, std::placeholders::_1));
}

void Hitmarker::draw()
{
	if (!config.get<bool>(vars.bDrawHitmarker))
		return;

	if (!game::localPlayer)
		return;

	if (!game::localPlayer->isAlive())
		return;

	int x, y;
	bool mode3D = config.get<bool>(vars.bDrawHitmarker3D);
	if (!mode3D)
	{
		x = globals::screenX / 2;
		y = globals::screenY / 2;
	}

	float currentAlpha = 0.0f;
	for (size_t i = 0; const auto & el : m_hitmarkers)
	{
		float diff = el.m_expire - interfaces::globalVars->m_curtime;

		if (diff < 0.0f)
		{
			m_hitmarkers.erase(m_hitmarkers.begin() + i);
			continue;
		}

		if (mode3D)
		{
			if (ImVec2 s; imRender.worldToScreen(el.m_pos, s))
			{
				x = static_cast<int>(s.x);
				y = static_cast<int>(s.y);
			}
			else
				continue;
		}

		currentAlpha = diff / config.get<float>(vars.fHitmarkerTime);
		float sizeFont = 16.0f;
		CfgColor actualColor = config.get<CfgColor>(vars.cDrawHitmarkerNormal).getColor().getColorEditAlpha(currentAlpha);
		float lineX = 10.0f;
		float lineY = 5.0f;

		if (el.isAvailable() && el.m_head)
		{
			actualColor = config.get<CfgColor>(vars.cDrawHitmarkerHead).getColor().getColorEditAlpha(currentAlpha);
			sizeFont = 24.0f;
			lineX = 14.0f;
			lineY = 7.0f;
		}
		else if (!el.isAvailable())
		{
			actualColor = config.get<CfgColor>(vars.cDrawHitmarkerDead).getColor().getColorEditAlpha(currentAlpha);
			sizeFont = 28.0f;
			lineX = 18.0f;
			lineY = 9.0f;
		}

		float lineAddonX = lineX;
		float lineAddonY = lineY;
		if (config.get<bool>(vars.bDrawHitmarkerResize))
		{
			lineAddonX = lineX / (1.0f / (currentAlpha + 0.01f)); // prevent division by 0 and make ratio
			lineAddonY = lineY / (1.0f / (currentAlpha + 0.01f));
		}

		imRender.drawLine(x - lineAddonX, y + lineAddonX, x - lineAddonY, y + lineAddonY, actualColor.getColor());
		imRender.drawLine(x + lineAddonX, y + lineAddonX, x + lineAddonY, y + lineAddonY, actualColor.getColor());
		imRender.drawLine(x - lineAddonX, y - lineAddonX, x - lineAddonY, y - lineAddonY, actualColor.getColor());
		imRender.drawLine(x + lineAddonX, y - lineAddonX, x + lineAddonY, y - lineAddonY, actualColor.getColor());

		constexpr int moveMultiply = 25;
		float correction = (1.0f - currentAlpha) * moveMultiply; // this maybe should have el.expire ratio to previous one
		float Xcorrection = x + 8.0f + (correction * 0.6f); // multiply 0.6 to get a bit niver effect, 8 comes from padding
		float Ycorrection = y - (correction * 4.0f); // 4.0f comes from hardcoding. Make it more nice, maybe there are better ways for this

		imRender.text(Xcorrection, Ycorrection, sizeFont, ImFonts::tahoma14, FORMAT(XOR("{}"), el.m_dmg), false, actualColor.getColor(), false);

		i++;
	}
}

void Hitmarker::handleHits(IGameEvent* event)
{
	if (!config.get<bool>(vars.bDrawHitmarker))
		return;

	auto attacker = interfaces::entList->getClientEntity(interfaces::engine->getPlayerID(event->getInt(XOR("attacker"))));
	if (!attacker)
		return;

	// very important
	if (attacker != game::localPlayer)
		return;

	auto ent = reinterpret_cast<Player_t*>(interfaces::entList->getClientEntity(interfaces::engine->getPlayerID(event->getInt(XOR("userid")))));
	if (!ent) // should never happen
		return;

	auto dmg_health = event->getInt(XOR("dmg_health"));
	auto health = ent->m_iHealth() - dmg_health;
	auto hitgroup = event->getInt(XOR("hitgroup"));

	Hitmark_t hit =
	{
		interfaces::globalVars->m_curtime + config.get<float>(vars.fHitmarkerTime),
		dmg_health,
		ent->m_iHealth() - dmg_health,
		hitgroup == 1, // head
		ent->getHitgroupPos(hitgroup)
	};
	m_hitmarkers.push_back(hit);
	if (config.get<bool>(vars.bPlayHitmarker))
		interfaces::surface->playSound(XOR("buttons\\arena_switch_press_02.wav"));
}
