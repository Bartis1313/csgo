#include "hitmark.hpp"

#include <SDK/CGlobalVars.hpp>
#include <SDK/IGameEvent.hpp>
#include <SDK/IVEngineClient.hpp>
#include <SDK/IClientEntityList.hpp>
#include <SDK/ISurface.hpp>
#include <SDK/interfaces/interfaces.hpp>
#include <gamememory/memory.hpp>
#include <cheats/game/game.hpp>
#include <cheats/game/globals.hpp>
#include <config/vars.hpp>
#include <render/render.hpp>
#include <utilities/tools/tools.hpp>
#include <utilities/tools/wrappers.hpp>
#include <cheats/features/events/events.hpp>

#include <functional>

// TODO: try if this is okay-ish https://gitlab.com/KittenPopo/csgo-2018-source/-/blob/main/game/shared/takedamageinfo.h#L24
// and then: https://gitlab.com/KittenPopo/csgo-2018-source/-/blob/main/game/server/player.cpp#L1725
// server.dll 55 8B EC 83 EC 18 56 57 8B 7D 08 8B F1 57
// retaddr to check 84 C0 74 34 8B 4D 08 8B 01 
void Hitmarker::init()
{
	events::add(XOR("player_hurt"), std::bind(&Hitmarker::handleHits, this, std::placeholders::_1));
}

void Hitmarker::draw()
{
	if (!vars::misc->hitmarker->enabled)
		return;

	if (!game::localPlayer)
		return;

	if (!game::localPlayer->isAlive())
		return;

	int x, y;
	bool mode3D = vars::misc->hitmarker->enabled3D;
	if (!mode3D)
	{
		x = globals::screenX / 2;
		y = globals::screenY / 2;
	}

	float currentAlpha = 0.0f;
	for (size_t i = 0; const auto & el : m_hitmarkers)
	{
		float diff = el.m_expire - memory::interfaces::globalVars->m_curtime;

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

		currentAlpha = diff / vars::misc->hitmarker->time;
		float sizeFont = 16.0f;
		Color actualColor = vars::misc->hitmarker->colorNormal().getColorEditAlpha(currentAlpha);
		float lineX = 10.0f;
		float lineY = 5.0f;

		if (el.isAvailable() && el.m_head)
		{
			actualColor = vars::misc->hitmarker->colorHead().getColorEditAlpha(currentAlpha);
			sizeFont = 24.0f;
			lineX = 14.0f;
			lineY = 7.0f;
		}
		else if (!el.isAvailable())
		{
			actualColor = vars::misc->hitmarker->colorDead().getColorEditAlpha(currentAlpha);
			sizeFont = 28.0f;
			lineX = 18.0f;
			lineY = 9.0f;
		}

		float lineAddonX = lineX;
		float lineAddonY = lineY;
		if (vars::misc->hitmarker->enabledResize)
		{
			lineAddonX = lineX / (1.0f / (currentAlpha + 0.01f)); // prevent division by 0 and make ratio
			lineAddonY = lineY / (1.0f / (currentAlpha + 0.01f));
		}

		imRender.drawLine(x - lineAddonX, y + lineAddonX, x - lineAddonY, y + lineAddonY, actualColor);
		imRender.drawLine(x + lineAddonX, y + lineAddonX, x + lineAddonY, y + lineAddonY, actualColor);
		imRender.drawLine(x - lineAddonX, y - lineAddonX, x - lineAddonY, y - lineAddonY, actualColor);
		imRender.drawLine(x + lineAddonX, y - lineAddonX, x + lineAddonY, y - lineAddonY, actualColor);

		constexpr int moveMultiply = 25;
		float correction = (1.0f - currentAlpha) * moveMultiply; // this maybe should have el.expire ratio to previous one
		float Xcorrection = x + 8.0f + (correction * 0.6f); // multiply 0.6 to get a bit niver effect, 8 comes from padding
		float Ycorrection = y - (correction * 4.0f); // 4.0f comes from hardcoding. Make it more nice, maybe there are better ways for this

		imRender.text(Xcorrection, Ycorrection, sizeFont, ImFonts::tahoma14, FORMAT(XOR("{}"), el.m_dmg), false, actualColor, false);

		i++;
	}
}

void Hitmarker::handleHits(IGameEvent* event)
{
	if (!vars::misc->hitmarker->enabled)
		return;

	auto attacker = memory::interfaces::entList->getClientEntity(memory::interfaces::engine->getPlayerID(event->getInt(XOR("attacker"))));
	if (!attacker)
		return;

	// very important
	if (attacker != game::localPlayer)
		return;

	auto ent = reinterpret_cast<Player_t*>(memory::interfaces::entList->getClientEntity(memory::interfaces::engine->getPlayerID(event->getInt(XOR("userid")))));
	if (!ent) // should never happen
		return;

	auto dmg_health = event->getInt(XOR("dmg_health"));
	auto health = ent->m_iHealth() - dmg_health;
	auto hitgroup = event->getInt(XOR("hitgroup"));

	Hitmark_t hit =
	{
		memory::interfaces::globalVars->m_curtime + vars::misc->hitmarker->time,
		dmg_health,
		ent->m_iHealth() - dmg_health,
		hitgroup == 1, // head
		ent->getHitgroupPos(hitgroup)
	};
	m_hitmarkers.push_back(hit);

	if (vars::misc->hitmarker->play)
		memory::interfaces::surface->playSound(XOR("buttons\\arena_switch_press_02.wav"));
}
