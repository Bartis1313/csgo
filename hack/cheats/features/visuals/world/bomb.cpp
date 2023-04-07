#include "bomb.hpp"

#include "../../events/events.hpp"
#include "../../cache/cache.hpp"

#include <SDK/CGlobalVars.hpp>
#include <SDK/IEffects.hpp>
#include <SDK/Enums.hpp>
#include <SDK/ClientClass.hpp>
#include <SDK/IBaseClientDll.hpp>
#include <SDK/IClientEntityList.hpp>
#include <SDK/IVEngineClient.hpp>
#include <SDK/IGameEvent.hpp>
#include <SDK/ICvar.hpp>
#include <SDK/ConVar.hpp>
#include <SDK/math/Vector.hpp>
#include <SDK/interfaces/interfaces.hpp>
#include <gamememory/memory.hpp>

#include <cheats/game/game.hpp>
#include <cheats/game/globals.hpp>
#include <config/vars.hpp>
#include <utilities/tools/tools.hpp>
#include <utilities/tools/wrappers.hpp>
#include <render/render.hpp>
#include <utilities/math/math.hpp>


void BombOverlay::init()
{
	m_timer = memory::interfaces::cvar->findVar("mp_c4timer");

	// if we seriously do not want to use events all
	// then find a way to get planted bomb info, I was lazy to do so
	events::add("bomb_planted", std::bind(&BombOverlay::handleWhoPlanted, this, std::placeholders::_1));
}

void BombOverlay::roundRestart()
{
	g_BombOverlay->m_bombEnt = nullptr;
}

void BombOverlay::draw()
{
	if (!game::isAvailable())
		m_bombEnt = nullptr;

	if (!m_bombEnt)
		return;

	if (!vars::visuals->world->bomb->enabled)
		return;

	const auto bombent = reinterpret_cast<Bomb_t*>(m_bombEnt);
	const auto bombtime = bombent->m_flC4Blow() - memory::interfaces::globalVars->m_curtime;
	const auto defusetime = m_bombEnt->m_flDefuseCountDown() - memory::interfaces::globalVars->m_curtime;
	auto ent = reinterpret_cast<Player_t*>(memory::interfaces::entList->getClientFromHandle(m_bombEnt->m_hBombDefuser()));
	const auto defuseMaxTime = ent && ent->m_bHasDefuser() ? 5.0f : 10.0f; // check ent too

	if (bombtime < 0.0f || bombent->m_bBombDefused())
	{
		m_bombEnt = nullptr;
		return;
	}

	// https://www.unknowncheats.me/forum/counterstrike-global-offensive/389530-bomb-damage-indicator.html
	constexpr float bombRadius = 500.0f; // there is no info for this, run some map scanner
	constexpr float sigma = (500.0f * 3.5f) / 3.0f;
	const float hypDist = (m_bombEnt->getEyePos() - game::localPlayer->getEyePos()).length();
	const float dmg = game::scaleDamageArmor((bombRadius * (std::exp(-hypDist * hypDist / (2.0f * sigma * sigma)))), static_cast<float>(game::localPlayer->m_ArmorValue()));
	const bool isSafe = dmg < game::localPlayer->m_iHealth();

	float scaled = m_bombEnt->m_hBombDefuser().isValid() ? (defusetime / defuseMaxTime) : (bombtime / m_timer->getFloat());

	float fdef = defusetime / defuseMaxTime;
	float fbomb = bombtime / m_timer->getFloat();
	float forRed = m_bombEnt->m_hBombDefuser().isValid() ? fdef : fbomb;
	float forGreen = m_bombEnt->m_hBombDefuser().isValid() ? fdef : fbomb;

	float r = (1.0f - forRed);
	Color color{ r, forGreen, 0.0f, 0.8f };

	ImGui::SetNextWindowSize({ 300.0f, 150.0f });
	ImGui::PushStyleColor(ImGuiCol_WindowBg, Color::U32(vars::visuals->world->bomb->background()));
	if (ImGui::Begin("Bomb c4", &vars::visuals->world->bomb->enabled, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize))
	{
		const auto size = ImGui::GetWindowSize();
		const auto pos = ImGui::GetWindowPos();
		const auto draw = ImGui::GetWindowDrawList();

		constexpr float minAngle = -90.0f;
		const float maxAngle = math::RAD2DEG(math::PI * 2.0f * scaled) + minAngle;

		const float scaledRadius = std::min(size.x, size.y) / 4.0f;
		const auto xCircle = (size.x - scaledRadius) * 0.95f;
		const auto yCircle = size.y / 2.0f;

		drawing::Arc{ ImVec2{ pos.x + xCircle, pos.y + yCircle }, scaledRadius, math::DEG2RAD(minAngle), math::DEG2RAD(maxAngle), 32, Color::U32(color), 0, 5.0f }.draw(draw);
		const auto text = defusetime > bombtime ? "Too late" : std::format("{:.2f}", m_bombEnt->m_hBombDefuser().isValid() ? defusetime : bombtime);
		drawing::Text{ ImFonts::tahoma14, ImVec2{ pos.x + xCircle, pos.y + yCircle - ImFonts::tahoma14->FontSize / 2.0f },
			Color::U32(Colors::White), text, false, true }.draw(draw);

		const auto icon = game::getWeaponIcon("weapon_c4");
		drawing::Image{ icon.texture, ImVec2{ pos.x + 5.0f, pos.y + 5.0f }, ImVec2{ pos.x + 5.0f + 30.0f, pos.y + 5.0f + 40.0f },
			ImVec2{ 0.0f, 0.0f }, ImVec2{ 1.0f, 1.0f }, Color::U32(Colors::White) }.draw(draw);

		float yPosInfo = pos.y + size.y / 2.0f;

		if (!m_whoPlanted.empty())
		{
			drawing::Text{ ImFonts::tahoma20, ImVec2{ pos.x + 5.0f, yPosInfo }, Color::U32(Colors::White),
			std::format("Planted by {}s", m_whoPlanted), false, false }.draw(draw);
			yPosInfo -= ImFonts::tahoma20->FontSize;
		}
		if (m_bombEnt->m_hBombDefuser().isValid())
		{
			drawing::Text{ ImFonts::tahoma20, ImVec2{ pos.x + 5.0f, yPosInfo }, Color::U32(Colors::White),
			std::format("Defusing {}", ent->getName()), false, false }.draw(draw);
			yPosInfo -= ImFonts::tahoma20->FontSize;
		}

		float yCentreInfo = pos.y + 2.0f + ImFonts::tahoma20->FontSize;
		drawing::Text{ ImFonts::tahoma20, ImVec2{ pos.x + size.x / 2.0f, yCentreInfo }, Color::U32(Colors::White), std::format("Site {}",
			m_bombEnt->getBombSiteName()), false, true }.draw(draw);
		yCentreInfo += ImFonts::tahoma20->FontSize;
		drawing::Text{ ImFonts::tahoma20, ImVec2{ pos.x + size.x / 2.0f, yCentreInfo }, Color::U32(isSafe ? Colors::Green : Colors::Red),
			std::format("Damage {:.2f}", dmg), false, true }.draw(draw);

		ImGui::End();
	}
	ImGui::PopStyleColor();
}

void BombOverlay::handleWhoPlanted(IGameEvent* event)
{
	auto who = reinterpret_cast<Player_t*>(memory::interfaces::entList->getClientEntity(memory::interfaces::engine->getPlayerID(event->getInt("userid"))));
	if (!who)
		return;

	m_whoPlanted = who->getName();
}

void BombOverlayEntGrabber::run(int frame)
{
	if (frame != FRAME_RENDER_START)
		return;

	for (auto [entity, idx, classID] : EntityCache::getCache(EntCacheType::WORLD_ENTS))
	{
		if (classID != CPlantedC4)
			continue;

		g_BombOverlay->m_bombEnt = reinterpret_cast<Bomb_t*>(entity);
	}
}