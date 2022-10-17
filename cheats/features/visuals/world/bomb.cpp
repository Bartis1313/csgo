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

#include <game/game.hpp>
#include <game/globals.hpp>
#include <config/vars.hpp>
#include <utilities/tools/tools.hpp>
#include <utilities/tools/wrappers.hpp>
#include <utilities/renderer/renderer.hpp>


void BombOverlay::init()
{
	m_timer = interfaces::cvar->findVar(XOR("mp_c4timer"));

	g_Events->add(XOR("bomb_planted"), std::bind(&BombOverlay::handleWhoPlanted, this, std::placeholders::_1));
	g_Events->add(XOR("bomb_exploded"), std::bind(&BombOverlay::handleBombExplode, this, std::placeholders::_1));
	g_Events->add(XOR("round_prestart"), std::bind(&BombOverlay::handleResetBomb, this, std::placeholders::_1));
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
	const auto bombtime = bombent->m_flC4Blow() - interfaces::globalVars->m_curtime;
	const auto defusetime = m_bombEnt->m_flDefuseCountDown() - interfaces::globalVars->m_curtime;
	auto ent = reinterpret_cast<Player_t*>(interfaces::entList->getClientFromHandle(m_bombEnt->m_hBombDefuser()));
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

	float scaled = m_bombEnt->m_hBombDefuser() > 0 ? (defusetime / defuseMaxTime) : (bombtime / m_timer->getFloat());

	float fdef = defusetime / defuseMaxTime;
	float fbomb = bombtime / m_timer->getFloat();
	float forRed = m_bombEnt->m_hBombDefuser() > 0 ? fdef : fbomb;
	float forGreen = m_bombEnt->m_hBombDefuser() > 0 ? fdef : fbomb;

	float r = (255.0f - forRed * 255.0f);
	float g = (forGreen * 255.0f);
	Color color{ static_cast<int>(r), static_cast<int>(g), 0, 200 };

	constexpr ImVec2 size = { 300, 150 };
	ImGui::SetNextWindowSize(size);
	ImGui::PushStyleColor(ImGuiCol_WindowBg, Color::U32(vars::visuals->world->bomb->background()));
	if (ImGui::Begin(XOR("Bomb c4"), &vars::visuals->world->bomb->enabled, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize))
	{
		imRenderWindow.addList();

		constexpr float width = size.x;
		constexpr float height = size.y;
		constexpr float radius = height / 3.0f;

		constexpr float scaledX = (width * 0.95f) - radius;
		constexpr float scaledY = height / 2.0f;

		imRenderWindow.drawProgressRing(scaledX, scaledY, radius, 32, -90, scaled, 5.0f, color);
		if (defusetime > bombtime)
			imRenderWindow.drawText(scaledX, scaledY - (radius / 2.5f / 2.0f), radius / 2.5f, ImFonts::franklinGothic30, XOR("Too late"), true, Colors::White, false);
		else
			imRenderWindow.drawText(scaledX, scaledY - (radius / 2.5f / 2.0f), radius / 2.5f, ImFonts::franklinGothic30, FORMAT(XOR("{:.2f}"),
				m_bombEnt->m_hBombDefuser() > 0 ? defusetime : bombtime), true, Colors::White, false);
		imRenderWindow.drawText(2.0f, 0.0f, height / 2.0f, ImFonts::icon, u8"\uE031"_u8str, false, Colors::White, false);
		if (!m_whoPlanted.empty())
			imRenderWindow.drawText(2.0f, height / 2.0f + 2, 15, ImFonts::franklinGothic30, FORMAT(XOR("Planted by {}s"), m_whoPlanted), false, Colors::White, false);
		if (m_bombEnt->m_hBombDefuser() > 0)
			imRenderWindow.drawText(2.0f, height / 2.0f + 18, 15, ImFonts::franklinGothic30, FORMAT(XOR("Defusing {}"), ent->getName()), false, Colors::White, false);
		imRenderWindow.drawText(width / 2.0f, 2.0f, 15, ImFonts::franklinGothic30, FORMAT(XOR("Site {}"), m_bombEnt->getBombSiteName()), true, Colors::White, false);
		imRenderWindow.drawText(width / 2.0f, 20.0f, 15, ImFonts::franklinGothic30, FORMAT(XOR("Damage {:.2f}"), dmg), true, isSafe ? Colors::Green : Colors::Red, false);

		imRenderWindow.end();
		ImGui::End();
	}
	ImGui::PopStyleColor();
}

void BombOverlay::handleBombExplode(IGameEvent* event)
{
	m_bombEnt = nullptr;
}

void BombOverlay::handleWhoPlanted(IGameEvent* event)
{
	auto who = reinterpret_cast<Player_t*>(interfaces::entList->getClientEntity(interfaces::engine->getPlayerID(event->getInt(XOR("userid")))));
	if (!who)
		return;

	m_whoPlanted = who->getName();
}

void BombOverlay::handleResetBomb(IGameEvent* event)
{
	m_bombEnt = nullptr;
}

void BombOverlayEntGrabber::init()
{

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