#include "drawinfo.hpp"

#include <SDK/IVEngineClient.hpp>
#include <SDK/IWeapon.hpp>
#include <SDK/interfaces/interfaces.hpp>
#include <SDK/IVEngineClient.hpp>
#include <SDK/IClientEntityList.hpp>
#include <game/game.hpp>
#include <game/globals.hpp>
#include <config/vars.hpp>
#include <utilities/tools/tools.hpp>
#include <utilities/tools/wrappers.hpp>
#include <utilities/renderer/renderer.hpp>
#include <features/events/events.hpp>

void MiscInfo::init()
{
	if (game::isAvailable())
	{
		m_allHits = game::localPlayer->m_totalHitsOnServer(); // those gets clamped at 255 :(
		// https://gitlab.com/KittenPopo/csgo-2018-source/-/blob/main/game/shared/cstrike15/cs_player_shared.cpp#L1682
		// ida: direct reference: [actual address in first opcode] E8 ? ? ? ? 8B 44 24 24 46 
	}

	events::add(XOR("player_hurt"), std::bind(&MiscInfo::addHits, this, std::placeholders::_1));
	events::add(XOR("round_end"), std::bind(&MiscInfo::resetHits, this, std::placeholders::_1));
}

void MiscInfo::addHits(IGameEvent* event)
{
	auto attacker = memory::interfaces::entList->getClientEntity(memory::interfaces::engine->getPlayerID(event->getInt(XOR("attacker"))));
	if (!attacker)
		return;

	// very important
	if (attacker != game::localPlayer)
		return;

	auto ent = reinterpret_cast<Player_t*>(memory::interfaces::entList->getClientEntity(memory::interfaces::engine->getPlayerID(event->getInt(XOR("userid")))));
	if (!ent) // should never happen
		return;

	++m_allHits;
}

void MiscInfo::resetHits(IGameEvent* event)
{
	m_allHits = 0;
}

void MiscInfo::draw()
{
	if (!vars::misc->info->enabled)
		return;

	if (!game::isAvailable())
		return;

	const auto weapon = game::localPlayer->getActiveWeapon();
	if (!weapon)
		return;

	auto text = []
	(const std::string& text, const Color& color)
	{
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4{ color.r(), color.g(), color.b(), color.a() });
		ImGui::TextUnformatted(text.c_str()); // because textcolored is formatted by va args
		ImGui::PopStyleColor();
	};

	if (ImGui::Begin(XOR("##info"), nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar
		| ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoScrollbar))
	{
		text(FORMAT(XOR("Map: {}"), memory::interfaces::engine->getLevelName()), Colors::Green);
		text(FORMAT(XOR("Weapon {}"), weapon->getWpnInfo()->m_WeaponName), Colors::Yellow);
		text(FORMAT(XOR("[{} / {}] DMG [{}HP]"), weapon->m_iClip1(), weapon->m_iPrimaryReserveAmmoCount(), weapon->getWpnInfo()->m_damage), Colors::Yellow);
		text(FORMAT(XOR("Current In-accuracy {:.2f}%"), weapon->getInaccuracy() * 100.0f), Colors::Yellow);
		text(FORMAT(XOR("Zoom level {}"), weapon->m_zoomLevel()), Colors::Yellow);
		text(FORMAT(XOR("POS: X {:.2f} Y {:.2f} Z {:.2f}"), game::localPlayer->absOrigin()[Coord::X], game::localPlayer->absOrigin()[Coord::Y], game::localPlayer->absOrigin()[Coord::Z]), Colors::Yellow);
		text(FORMAT(XOR("Velocity {:.2f}"), game::localPlayer->m_vecVelocity().toVecPrev().length()), Colors::Yellow);
		text(FORMAT(XOR("Kills {}"), game::localPlayer->getKills()), Colors::Yellow);
		text(FORMAT(XOR("Deaths {}"), game::localPlayer->getDeaths()), Colors::Yellow);
		// escape divide by zero exceptions by using this trick
		float kd = game::localPlayer->getKills() / (game::localPlayer->getDeaths() ? game::localPlayer->getDeaths() : 1.0f);
		float kpm = game::localPlayer->getKills() / (game::serverTime() / 60.0f);
		text(FORMAT(XOR("KD {:.2f} KPM: {:.2f}"), kd, kpm), Colors::Yellow);
		text(FORMAT(XOR("Ping {}"), game::localPlayer->getPing()), Colors::Yellow);
		float accuracy = game::localPlayer->m_vecBulletVerifyListClient().m_size
			? (static_cast<float>(m_allHits) / static_cast<float>(game::localPlayer->m_vecBulletVerifyListClient().m_size)) * 100.0f
			: 0.0f;
		float fixedKills = game::localPlayer->getKills() ? game::localPlayer->getKills() : 1.0f;
		float hs = game::localPlayer->m_iNumRoundKillsHeadshots()
			? (static_cast<float>(game::localPlayer->m_iNumRoundKillsHeadshots()) / fixedKills) * 100.0f
			: 0.0f;
		text(FORMAT(XOR("Accuracy [{} / {}] {:.2f}% HS {:.2f}%"),
			m_allHits, game::localPlayer->m_vecBulletVerifyListClient().m_size, accuracy, hs), Colors::Yellow);

		ImGui::End();
	}
}