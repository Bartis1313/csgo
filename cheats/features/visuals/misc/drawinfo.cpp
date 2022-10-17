#include "drawinfo.hpp"

#include <SDK/IVEngineClient.hpp>
#include <SDK/IWeapon.hpp>
#include <SDK/interfaces/interfaces.hpp>
#include <game/game.hpp>
#include <game/globals.hpp>
#include <config/vars.hpp>
#include <utilities/tools/tools.hpp>
#include <utilities/tools/wrappers.hpp>
#include <utilities/renderer/renderer.hpp>

void MiscInfo::draw()
{
	if (!vars::misc->info->enabled)
		return;

	if (!game::localPlayer)
		return;

	if (!interfaces::engine->isInGame())
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
		text(FORMAT(XOR("Map: {}"), interfaces::engine->getLevelName()), Colors::Green);
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
		float accuracy = globals::shotsFired
			? (static_cast<float>(globals::shotsHit) / static_cast<float>(globals::shotsFired)) * 100.0f
			: 0.0f;
		float fixedKills = game::localPlayer->getKills() ? game::localPlayer->getKills() : 1.0f;
		float hs = globals::shotsHead
			? (static_cast<float>(globals::shotsHead) / fixedKills) * 100.0f
			: 0.0f;
		text(FORMAT(XOR("Accuracy [{} / {}] {:.2f}% HS {:.2f}%"), globals::shotsHit, globals::shotsFired, accuracy, hs), Colors::Yellow);

		ImGui::End();
	}
}