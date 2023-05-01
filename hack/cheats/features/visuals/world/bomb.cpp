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

#include <cheats/hooks/present.hpp>
#include <cheats/hooks/frameStageNotify.hpp>
#include <cheats/hooks/unkownRoundEnd.hpp>

namespace
{
	struct BombDrawer : hooks::Present
	{
		BombDrawer()
		{
			this->registerRun(bombOverlay::draw);
		}
	} bombDrawer;

	struct BombEntHandler : hooks::FrameStageNotify
	{
		BombEntHandler()
		{
			this->registerInit(bombOverlay::init);
			this->registerRun(bombOverlay::run);
		}
	} bombEntHandler;

	struct BombReset : hooks::UnknownRoundEnd
	{
		BombReset()
		{
			this->registerRun(bombOverlay::roundRestart);
		}
	} bombReset;
}

namespace bombOverlay
{
	Bomb_t* bombEnt{ };
	IConVar* c4Timer{ };
	std::string whoPlanted{ };
}

void bombOverlay::init()
{
	c4Timer = memory::interfaces::cvar->findVar("mp_c4timer");

	// if we seriously do not want to use events all
	// then find a way to get planted bomb info, I was lazy to do so
	events::add("bomb_planted", std::bind(&handleWhoPlanted, std::placeholders::_1));
}

void bombOverlay::roundRestart()
{
	bombEnt = nullptr;
}

void bombOverlay::draw()
{
	if (!game::isAvailable())
		bombEnt = nullptr;

	if (!bombEnt)
		return;

	if (!vars::visuals->world->bomb->enabled)
		return;

	const auto bombent = reinterpret_cast<Bomb_t*>(bombEnt);
	const auto bombtime = bombent->m_flC4Blow() - memory::interfaces::globalVars->m_curtime;
	const auto defusetime = bombEnt->m_flDefuseCountDown() - memory::interfaces::globalVars->m_curtime;
	auto ent = reinterpret_cast<Player_t*>(memory::interfaces::entList->getClientFromHandle(bombEnt->m_hBombDefuser()));
	const auto defuseMaxTime = ent && ent->m_bHasDefuser() ? 5.0f : 10.0f; // check ent too

	if (bombtime < 0.0f || bombent->m_bBombDefused())
	{
		bombEnt = nullptr;
		return;
	}

	// https://www.unknowncheats.me/forum/counterstrike-global-offensive/389530-bomb-damage-indicator.html
	constexpr float bombRadius = 500.0f; // there is no info for this, run some map scanner
	constexpr float sigma = (bombRadius * 3.5f) / 3.0f;
	const float hypDist = (bombEnt->getEyePos() - game::localPlayer->getEyePos()).length();
	const float dmg = game::scaleDamageArmor((bombRadius * (std::exp(-hypDist * hypDist / (2.0f * sigma * sigma)))), static_cast<float>(game::localPlayer->m_ArmorValue()));
	const bool isSafe = dmg < game::localPlayer->m_iHealth();

	float scaled = bombEnt->m_hBombDefuser().isValid() ? (defusetime / defuseMaxTime) : (bombtime / c4Timer->getFloat());

	float fdef = defusetime / defuseMaxTime;
	float fbomb = bombtime / c4Timer->getFloat();
	float forRed = bombEnt->m_hBombDefuser().isValid() ? fdef : fbomb;
	float forGreen = bombEnt->m_hBombDefuser().isValid() ? fdef : fbomb;

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
		const auto text = defusetime > bombtime ? "Too late" : std::format("{:.2f}", bombEnt->m_hBombDefuser().isValid() ? defusetime : bombtime);
		drawing::Text{ ImRender::fonts::tahoma14, ImVec2{ pos.x + xCircle, pos.y + yCircle - ImRender::fonts::tahoma14->FontSize / 2.0f },
			Color::U32(Colors::White), text, false, true }.draw(draw);

		const auto icon = game::getWeaponIcon("weapon_c4");
		drawing::Image{ icon.texture, ImVec2{ pos.x + 5.0f, pos.y + 5.0f }, ImVec2{ pos.x + 5.0f + 30.0f, pos.y + 5.0f + 40.0f },
			ImVec2{ 0.0f, 0.0f }, ImVec2{ 1.0f, 1.0f }, Color::U32(Colors::White) }.draw(draw);

		float yPosInfo = pos.y + size.y / 2.0f;

		if (!whoPlanted.empty())
		{
			drawing::Text{ ImRender::fonts::tahoma20, ImVec2{ pos.x + 5.0f, yPosInfo }, Color::U32(Colors::White),
			std::format("Planted by {}s", whoPlanted), false, false }.draw(draw);
			yPosInfo -= ImRender::fonts::tahoma20->FontSize;
		}
		if (bombEnt->m_hBombDefuser().isValid())
		{
			drawing::Text{ ImRender::fonts::tahoma20, ImVec2{ pos.x + 5.0f, yPosInfo }, Color::U32(Colors::White),
			std::format("Defusing {}", ent->getName()), false, false }.draw(draw);
			yPosInfo -= ImRender::fonts::tahoma20->FontSize;
		}

		float yCentreInfo = pos.y + 2.0f + ImRender::fonts::tahoma20->FontSize;
		drawing::Text{ ImRender::fonts::tahoma20, ImVec2{ pos.x + size.x / 2.0f, yCentreInfo }, Color::U32(Colors::White), std::format("Site {}",
			bombEnt->getBombSiteName()), false, true }.draw(draw);
		yCentreInfo += ImRender::fonts::tahoma20->FontSize;
		drawing::Text{ ImRender::fonts::tahoma20, ImVec2{ pos.x + size.x / 2.0f, yCentreInfo }, Color::U32(isSafe ? Colors::Green : Colors::Red),
			std::format("Damage {:.2f}", dmg), false, true }.draw(draw);

		ImGui::End();
	}
	ImGui::PopStyleColor();
}

void bombOverlay::handleWhoPlanted(IGameEvent* event)
{
	auto who = reinterpret_cast<Player_t*>(memory::interfaces::entList->getClientEntity(memory::interfaces::engine->getPlayerID(event->getInt("userid"))));
	if (!who)
		return;

	whoPlanted = who->getName();
}

void bombOverlay::run(FrameStage stage)
{
	if (stage != FRAME_RENDER_START)
		return;

	for (auto [entity, idx, classID] : EntityCache::getCache(EntCacheType::WORLD_ENTS))
	{
		if (classID != CPlantedC4)
			continue;

		bombEnt = reinterpret_cast<Bomb_t*>(entity);
	}
}