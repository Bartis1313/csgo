#include "logger.hpp"

#include <SDK/CGlobalVars.hpp>
#include <SDK/IVEngineClient.hpp>
#include <SDK/IClientEntityList.hpp>
#include <SDK/interfaces/interfaces.hpp>
#include <gamememory/memory.hpp>
#include <cheats/game/game.hpp>
#include <render/render.hpp>
#include <config/vars.hpp>
#include <cheats/features/events/events.hpp>
#include <cheats/game/globals.hpp>

void Logger::init()
{
	events::add("player_hurt", std::bind(&Logger::handleHits, this, std::placeholders::_1));
}

void Logger::add(const Log_t& log)
{
	if (!vars::misc->logs->enabled)
		return;

	m_logs.push_back(log);
}

void Logger::draw()
{
	if (!vars::misc->logs->enabled)
		return;

	constexpr float animation = 0.2f;
	constexpr float fineClip = 10.0f;

	for (size_t i = 0; const auto & el : m_logs)
	{
		float in = std::clamp((memory::interfaces::globalVars->m_curtime - el.m_timeLog) / animation, 0.10f, 1.0f);
		float out = std::clamp((memory::interfaces::globalVars->m_curtime - el.m_timeLog - vars::misc->logs->time) / animation, 0.0f, 1.0f);

		float alpha = in * (1.0f - out);
		if (alpha <= 0.0f)
		{
			m_logs.erase(m_logs.begin() + i);
			continue;
		}

		float x = in * fineClip - out * fineClip;
		float y = ((el.m_font->FontSize * i) + fineClip) * alpha;

		if (y > globals::screenY * 0.3f)
		{
			m_logs.pop_back();
			break;
		}

		drawing::Text text{ el.m_font, ImVec2{ x, y }, Color::U32(el.m_color.getColorEditAlpha(alpha)), el.m_text, false, false };
		text.draw(ImGui::GetBackgroundDrawList());

		i++;
	}
}

void Logger::handleHits(IGameEvent* event)
{
	auto attacker = memory::interfaces::entList->getClientEntity(memory::interfaces::engine->getPlayerID(event->getInt("attacker")));
	if (!attacker)
		return;

	// very important
	if (attacker != game::localPlayer)
		return;

	auto ent = reinterpret_cast<Player_t*>(memory::interfaces::entList->getClientEntity(memory::interfaces::engine->getPlayerID(event->getInt("userid"))));
	if (!ent) // should never happen
		return;

	auto dmg_health = event->getInt("dmg_health");
	auto health = ent->m_iHealth() - dmg_health;
	auto hitgroup = event->getInt("hitgroup");

	auto hitGroupToStr = [hitgroup]() -> std::string_view
	{
		switch (hitgroup)
		{
		case HITGROUP_HEAD:
			return "Head";
		case HITGROUP_CHEST:
			return "Chest";
		case HITGROUP_STOMACH:
			return "Belly";
		case HITGROUP_LEFTARM:
			return "Left arm";
		case HITGROUP_RIGHTARM:
			return "right arm";
		case HITGROUP_LEFTLEG:
			return "Left leg";
		case HITGROUP_RIGHTLEG:
			return "Right arm";
		default:
			return std::format("Unk {}", hitgroup);
		}
	};

	add(Log_t
		{
			.m_text = std::format("Hit {} for [{} hp {}hp left] dmg in {}", ent->getName(), dmg_health, health, hitGroupToStr()),
			.m_color = Colors::Cyan,
			.m_timeLog = memory::interfaces::globalVars->m_curtime,
			.m_font = ImFonts::tahoma14
		});
}
