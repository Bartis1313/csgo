#include "player.hpp"

#include "boxes.hpp"
#include "enemyWarn.hpp"
#include "sounds.hpp"

#include "../../backtrack/backtrack.hpp"
#include "../../events/events.hpp"
#include "../../cache/cache.hpp"

#include <SDK/CGlobalVars.hpp>
#include <SDK/IClientEntityList.hpp>
#include <SDK/IVModelInfo.hpp>
#include <SDK/IVEngineClient.hpp>
#include <SDK/IVEffects.hpp>
#include <SDK/IGameEvent.hpp>
#include <SDK/IViewRenderBeams.hpp>
#include <SDK/animations.hpp>
#include <SDK/ICvar.hpp>
#include <SDK/ConVar.hpp>
#include <SDK/IWeapon.hpp>
#include <SDK/ILocalize.hpp>
#include <SDK/structs/Entity.hpp>
#include <SDK/interfaces/interfaces.hpp>
#include <gamememory/memory.hpp>
#include <config/vars.hpp>
#include <cheats/game/game.hpp>
#include <render/render.hpp>
#include <utilities/math/math.hpp>
#include <cheats/game/globals.hpp>

#include <ranges>

#include <cheats/hooks/paintTraverse.hpp>
#include <cheats/hooks/unkownRoundEnd.hpp>

namespace
{
	struct PlayerVisualsHandler : hooks::PaintTraverse
	{
		PlayerVisualsHandler()
		{
			this->registerRender(playerVisuals::draw);
		}
	} playerVisualsHandler;

	struct PlayerVisualsReset : hooks::UnknownRoundEnd
	{
		PlayerVisualsReset()
		{
			this->registerRun(playerVisuals::roundRestart);
		}
	} playerVisualsReset;
}

namespace playerVisuals
{
	struct DormacyInfo_t
	{
		Vec3 lastPos;
		float alpha;
		float lastUpdate;
		bool reset;

		[[nodiscard]] bool isValid() const
		{
			if (this->reset)
				return false;

			return true;
		}
	};

	struct VisualEntity
	{
		Player_t* entity;
		Box box;
		DormacyInfo_t dormacyInfo;
		float health; // handled on own
		float armor; // handled on own
	};

	struct Paddings
	{
		float top{ 0.0f };
		float bottom{ 0.0f };
		float left{ 0.0f };
		float right{ 0.0f };
	};

	std::array<VisualEntity, 65> players{ };
	Paddings padding{ };

	void drawBoxes(VisualEntity& player);
	void drawnName(VisualEntity& player);
	void runDLight(VisualEntity& player);
	void drawHealth(VisualEntity& player);
	void drawArmor(VisualEntity& player);
	void drawFlags(VisualEntity& player);
	void drawWeapon(VisualEntity& player);
	void drawSkeleton(VisualEntity& player);
	void drawSnapLine(VisualEntity& player);
	void drawLaser(VisualEntity& player);
	void updateDormacy(VisualEntity& player);
}

static float getDormantAlpha(const playerVisuals::DormacyInfo_t& dormacy, float originalCfg)
{
	if (dormacy.alpha > originalCfg)
		return originalCfg;
	else if (dormacy.alpha < originalCfg)
		return dormacy.alpha;

	return originalCfg;
}

void playerVisuals::draw()
{
	if (!game::isAvailable())
		return;

	bool isFlashOk{ true };

	if (game::localPlayer->m_flFlashDuration() > 0.0f)
	{
		if (game::localPlayer->m_flFlashBangTime() >= vars::visuals->esp->checks->flashLimit)
			isFlashOk = false;
	}

	for (auto [entity, idx, classID] : EntityCache::getCache(EntCacheType::PLAYER))
	{
		const auto player = reinterpret_cast<Player_t*>(entity);

		if (player == game::localPlayer)
			continue;

		if (!player->isAlive())
			continue;

		if (!player->isOtherTeam(game::localPlayer()))
			continue;

		auto& visual = players.at(idx);
		visual.entity = player;

		updateDormacy(visual);

		sound::findBest(player);
		enemyWarning::beginThink(player);

		if (!visual.dormacyInfo.isValid())
			continue;

		Box box{ player };
		if (!box.isValid())
			continue;

		if (!isFlashOk)
			continue;

		visual.box = box;

		if (vars::visuals->esp->checks->visible)
		{
			const bool check1 = game::localPlayer->isPossibleToSee(player, player->getHitboxPos(HITBOX_HEAD));
			const bool check2 = game::localPlayer->isPossibleToSee(player, player->getHitboxPos(HITBOX_BELLY));

			if (!check1 && !check2)
				continue;
		}

		if (vars::visuals->esp->checks->smoke)
		{
			const bool check1 = game::localPlayer->isViewInSmoke(player->getHitboxPos(HITBOX_HEAD));
			const bool check2 = game::localPlayer->isViewInSmoke(player->getHitboxPos(HITBOX_BELLY));

			if (!check1 && !check2)
				continue;
		}

		if (vars::visuals->esp->checks->dead)
		{
			if (game::localPlayer->isAlive())
				continue;
		}
		
		drawBoxes(visual);
		drawnName(visual);
		runDLight(visual);
		drawSkeleton(visual);
		drawSnapLine(visual);
		drawLaser(visual);
		drawHealth(visual);
		drawArmor(visual);
		drawWeapon(visual);
		drawFlags(visual);

		padding = Paddings{ };
	}

	sound::draw();
	enemyWarning::draw();

	enemyWarning::endThink();
}

void playerVisuals::drawBoxes(VisualEntity& player)
{
	if (vars::visuals->esp->boxes->box.mode == E2T(BoxTypes::OFF))
		return;

	padding.bottom += 4.0f;
	padding.top -= 2.0f;
	padding.left -= 4.0f;
	padding.right += 4.0f;

	const auto box = player.box;
	auto cfg = vars::visuals->esp->boxes->box;
	const float limitAlphaOutline = std::max(cfg.color().a(), cfg.fill().a());

	if (cfg.mode == E2T(BoxTypes::BOX2D))
	{
		const bool outlined = cfg.outline;

		if (outlined)
		{
			boxes::drawInnerOutline(box, getDormantAlpha(player.dormacyInfo, limitAlphaOutline));
			boxes::drawOuterOutline(box, getDormantAlpha(player.dormacyInfo, limitAlphaOutline));
		}

		boxes::drawRect(box, cfg.color.getColor(), getDormantAlpha(player.dormacyInfo, cfg.color().a()));
	}
	else if (cfg.mode == E2T(BoxTypes::FILLED2D))
	{
		const bool outlined = cfg.outline;
		const bool gradient = cfg.gradient;

		if (gradient)
		{
			ImRender::drawRectFilledMultiColor(box.x, box.y, box.w, box.h,
				cfg.gradientCol1().getColorEditAlpha(getDormantAlpha(player.dormacyInfo, cfg.gradientCol1().a())),
				cfg.gradientCol2().getColorEditAlpha(getDormantAlpha(player.dormacyInfo, cfg.gradientCol2().a())),
				cfg.gradientCol3().getColorEditAlpha(getDormantAlpha(player.dormacyInfo, cfg.gradientCol3().a())),
				cfg.gradientCol4().getColorEditAlpha(getDormantAlpha(player.dormacyInfo, cfg.gradientCol4().a())));
		}
		else
			boxes::drawRectFilled(box, cfg.fill(), getDormantAlpha(player.dormacyInfo, cfg.fill().a()));

		if (outlined)
		{
			boxes::drawOuterOutline(box, getDormantAlpha(player.dormacyInfo, Colors::Black.a()));
		}
	}
	else if (cfg.mode == E2T(BoxTypes::BOX3D))
	{
		boxes::drawBox3D(box, cfg.color(), getDormantAlpha(player.dormacyInfo, cfg.color().a()));
	}
	else if (cfg.mode == E2T(BoxTypes::FILLED3D))
	{
		boxes::drawBoxFilled3D(box, cfg.fill(), getDormantAlpha(player.dormacyInfo, cfg.fill().a()));
		boxes::drawBox3D(box, cfg.fill(), getDormantAlpha(player.dormacyInfo, cfg.fill().a()));
	}
}

void playerVisuals::drawnName(VisualEntity& player)
{
	if (!vars::visuals->esp->nameBar->enabled)
		return;

	const float fontSize = game::getScaledFont(player.entity->absOrigin(), game::localPlayer()->absOrigin());

	ImRender::text(player.box.x + player.box.w / 2.0f, player.box.y - fontSize - 2.0f, fontSize, ImRender::fonts::verdana12, player.entity->getName(), true,
		Color::healthBased(player.entity->m_iHealth()).getColorEditAlpha(player.dormacyInfo.alpha), false);
}

void playerVisuals::runDLight(VisualEntity& player)
{
	// https://github.com/ValveSoftware/source-sdk-2013/blob/0d8dceea4310fde5706b3ce1c70609d72a38efdf/sp/src/game/client/c_spotlight_end.cpp#L112

	if (!vars::visuals->esp->dlight->enabled)
		return;

	// in theory this should boost performance, in reality nope
	// the real performance would be rebuild of dlight not using many useless allocations
	//constexpr auto lightDie = 1e4f;
	//static std::array<DLight_t*, 128> dlights;
	//static std::once_flag once;
	//std::call_once(once, [&]()
	//	{
	//		for (size_t i = 0; i < dlights.size(); ++i)
	//		{
	//			dlights.at(i) = memory::interfaces::efx->clAllocDLight(i);
	//			// so this will 
	//			dlights.at(i)->m_die = memory::interfaces::globalVars->m_curtime + lightDie;
	//		}
	//	});

	//auto& dLight = dlights.at(player.entity->getIndex());
	//if (dLight->m_die == memory::interfaces::globalVars->m_curtime)
	//{
	//	// reallocate the light
	//	dLight = memory::interfaces::efx->clAllocDLight(player.entity->getIndex());
	//	dLight->m_die = memory::interfaces::globalVars->m_curtime + lightDie;
	//}

	auto dLight = memory::interfaces::efx->clAllocDLight(player.entity->getIndex());
	dLight->m_color = vars::visuals->esp->dlight->color();
	dLight->m_origin = player.entity->m_vecOrigin();
	dLight->m_radius = vars::visuals->esp->dlight->radius;
	dLight->m_die = memory::interfaces::globalVars->m_curtime + 0.1f;
	dLight->m_exponent = static_cast<char>(vars::visuals->esp->dlight->exponent);
	dLight->m_decay = vars::visuals->esp->dlight->decay;
	dLight->m_key = player.entity->getIndex();
}

// yoinked: https://www.unknowncheats.me/wiki/Counter_Strike_Global_Offensive:Bone_ESP
void playerVisuals::drawSkeleton(VisualEntity& player)
{
	if (!vars::visuals->esp->skeleton->enabled)
		return;

	const auto model = player.entity->getModel();
	if (!model)
		return;

	const auto studio = memory::interfaces::modelInfo->getStudioModel(model);
	if (!studio)
		return;

	const auto& btRecords = backtrack::records.at(player.entity->getIndex());

	auto skeletPos = [=](const size_t idx)
	{
		const auto child = btRecords.empty()
			? player.entity->getBonePos(idx)
			: btRecords.back().matrices[idx].origin();
		return child;
	};

	// bone IDs
	constexpr auto chest = 6;
	constexpr auto lowerChest = 5;

	for (auto i : std::views::iota(0, studio->m_bonesCount))
	{
		auto bone = studio->getBone(i);
		if (!bone)
			continue;

		if (bone->m_parent == -1)
			continue;

		if (!(bone->m_flags & BONE_USED_BY_HITBOX))
			continue;

		if (!btRecords.empty() && !backtrack::isValid(btRecords.front().simtime))
			continue;

		auto child = skeletPos(i);
		auto parent = skeletPos(bone->m_parent);
		auto chestbone = skeletPos(chest);
		auto upper = skeletPos(chest + 1) - chestbone;
		auto breast = chestbone + upper / 2.0f;

		auto deltachild = child - breast;
		auto deltaparent = parent - breast;

		if (deltaparent.length() < 9.0f && deltachild.length() < 9.0f)
			parent = breast;

		if (i == lowerChest)
			child = breast;

		if (std::abs(deltachild[Coord::Z]) < 5.0f && deltaparent.length() < 5.0f && deltachild.length() < 5.0f || i == chest)
			continue;

		if (vars::visuals->esp->skeleton->showDebug)
		{
			if (ImVec2 s; ImRender::worldToScreen(player.entity->getBonePos(i), s))
				ImRender::text(s.x, s.y, ImRender::fonts::franklinGothic12, std::format("{}", i), true, Colors::White, true);
		}

		if (ImVec2 start, end; ImRender::worldToScreen(parent, start) && ImRender::worldToScreen(child, end))
			ImRender::drawLine(start, end, vars::visuals->esp->skeleton->color().getColorEditAlpha(getDormantAlpha(player.dormacyInfo, vars::visuals->esp->skeleton->color().a())));
	}
}

void playerVisuals::drawHealth(VisualEntity& player)
{
	if (!vars::visuals->esp->healthBar->enabled)
		return;

	int maxHealth = player.entity->getMaxHealth();
	float frequency = maxHealth * (1.0f / 0.5f);

	auto& health = player.health;
	const auto realHealth = player.entity->m_iHealth();
	if (health > realHealth)
		health -= frequency * memory::interfaces::globalVars->m_frametime;
	else
		health = static_cast<float>(realHealth);

	const auto offset = health * player.box.h / maxHealth;
	const auto pad = player.box.h - offset;

	Box newBox
	{
		player.box.x + padding.left - 2.0f,
		player.box.y,
		2.0f,
		player.box.h,
	};

	ImRender::drawRectFilled(newBox.x - 1.0f, newBox.y - 1.0f, 4.0f, newBox.h + 2.0f, Colors::Black.getColorEditAlpha(getDormantAlpha(player.dormacyInfo, 1.0f)));
	ImRender::drawRectFilled(newBox.x, newBox.y + pad, 2.0f, offset, Color::healthBased(player.entity->m_iHealth()).getColorEditAlpha(getDormantAlpha(player.dormacyInfo, 1.0f)));

	// if the player has health below max, then draw HP info
	if (health < 100)
	{
		auto text = std::format("{}", realHealth);
		float fontSize = game::getScaledFont(player.entity->absOrigin(), game::localPlayer->absOrigin(), 100.0f, 10.0f, 14.0f);
		auto size = ImRender::fonts::franklinGothic12->CalcTextSizeA(fontSize, std::numeric_limits<float>::max(), 0.0f, text.c_str());

		ImRender::text(newBox.x - 4.0f - size.x, newBox.y + pad - 4.0f,
			fontSize, ImRender::fonts::franklinGothic12, text, false, Colors::White.getColorEditAlpha(player.dormacyInfo.alpha));
	}

	padding.left -= 6.0f;
}

void playerVisuals::drawArmor(VisualEntity& player)
{
	if (!vars::visuals->esp->armorBar->enabled)
		return;
	
	constexpr int maxArmor = 100;
	constexpr float frequency = maxArmor * (1.0f / 0.5f);
	auto& armor = player.armor;
	const auto realArmor = player.entity->m_ArmorValue();

	if (armor > realArmor)
		armor -= frequency * memory::interfaces::globalVars->m_frametime;
	else
		armor = static_cast<float>(realArmor);

	const auto offset = armor * player.box.h / maxArmor;
	const auto pad = player.box.h - offset;

	Box newBox
	{
		player.box.x + player.box.w + padding.right,
		player.box.y,
		2.0f,
		player.box.h,
	};

	const Color armorCol{ 0, static_cast<int>(armor * 1.4f), 255 };

	if (armor != 0)
	{
		ImRender::drawRectFilled(newBox.x - 1.0f, newBox.y - 1.0f, 4.0f, newBox.h + 2.0f, Colors::Black.getColorEditAlpha(getDormantAlpha(player.dormacyInfo, 1.0f)));
		ImRender::drawRectFilled(newBox.x, newBox.y + pad, 2.0f, offset, armorCol.getColorEditAlpha(getDormantAlpha(player.dormacyInfo, 1.0f)));
	}

	padding.right += 6.0f;
}

#include <SDK/ILocalize.hpp>

void playerVisuals::drawWeapon(VisualEntity& player)
{
	if (!vars::visuals->esp->weaponBar->enabled)
		return;

	const auto weapon = player.entity->getActiveWeapon();
	if (!weapon)
		return;

	Color tex = vars::visuals->esp->weaponBar->text();

	const int maxAmmo = weapon->getWpnInfo()->m_maxClip1;
	const int currentAmmo = weapon->m_iClip1();

	ImRender::text(player.box.x + player.box.w / 2, player.box.y + player.box.h + 5, ImRender::fonts::franklinGothic12, std::format("{} {}/{}",
		vars::visuals->esp->weaponBar->translate ? memory::interfaces::localize->findAsUTF8(weapon->getWpnInfo()->m_WeaponName) : player.entity->getActiveWeapon()->getWpnName(), currentAmmo, maxAmmo),
		true, vars::visuals->esp->weaponBar->text().getColorEditAlpha(getDormantAlpha(player.dormacyInfo, vars::visuals->esp->weaponBar->text().a())));

	// skip useless trash for calculations
	if (weapon->isNonAimable())
		return;

	Box newBox =
	{
		player.box.x,
		player.box.y + player.box.h + padding.bottom,
		player.box.w + 2.0f,
		2.0f
	};

	float barWidth = currentAmmo * player.box.w / maxAmmo;
	const auto animlayer = player.entity->getAnimOverlays()[1];

	if (animlayer.m_sequence)
	{
		const auto sequenceActivity = player.entity->getSequenceActivity(animlayer.m_sequence);
		const bool isReloading = sequenceActivity == 967 && animlayer.m_weight; // ACT_CSGO_RELOAD

		if (isReloading && animlayer.m_weight != 0.0f && animlayer.m_cycle < 0.99f)
			barWidth = (animlayer.m_cycle * player.box.w) / 1.0f;
	}

	const float alphaLimit = vars::visuals->esp->weaponBar->bar().a();

	ImRender::drawRectFilled(newBox.x - 1.0f, newBox.y - 1.0f, newBox.w, 4.0f, Colors::Black.getColorEditAlpha(getDormantAlpha(player.dormacyInfo, alphaLimit)));
	ImRender::drawRectFilled(newBox.x, newBox.y, barWidth, 2.0f, vars::visuals->esp->weaponBar->bar().getColorEditAlpha(getDormantAlpha(player.dormacyInfo, alphaLimit)));

	padding.bottom += 6.0f;
}

void playerVisuals::drawFlags(VisualEntity& player)
{
	std::vector<std::pair<std::string, Color>> flags{ };

	PlayerInfo_t info{ };
	if (!memory::interfaces::engine->getPlayerInfo(player.entity->getIndex(), &info))
		return;

	const auto cfgflags = vars::visuals->esp->flags->flags;

	if (cfgflags.at(E2T(EspFlags::BOT)))
	{
		if (info.m_fakePlayer)
			flags.emplace_back(std::make_pair("BOT", Colors::Yellow.getColorEditAlpha(getDormantAlpha(player.dormacyInfo, 1.0f))));
	}

	if (cfgflags.at(E2T(EspFlags::MONEY)))
	{
		flags.emplace_back(std::make_pair(std::format("{}$", player.entity->m_iAccount()), Colors::Green.getColorEditAlpha(getDormantAlpha(player.dormacyInfo, 1.0f))));
	}

	if (cfgflags.at(E2T(EspFlags::WINS)))
	{
		flags.emplace_back(std::make_pair(std::format("Wins {}", player.entity->getWins()), Colors::Green.getColorEditAlpha(getDormantAlpha(player.dormacyInfo, 1.0f))));
	}

	if (cfgflags.at(E2T(EspFlags::RANK)))
	{
		flags.emplace_back(std::make_pair(std::format("Rank {}", player.entity->getRank()), Colors::White.getColorEditAlpha(getDormantAlpha(player.dormacyInfo, 1.0f))));
	}

	if (cfgflags.at(E2T(EspFlags::ARMOR)))
	{
		std::string text{ };
		if (player.entity->m_bHasHelmet() && player.entity->m_ArmorValue())
			text = "H KEV";
		else if (!player.entity->m_bHasHelmet() && player.entity->m_ArmorValue())
			text = "KEV";

		flags.emplace_back(std::make_pair(text, Colors::White.getColorEditAlpha(getDormantAlpha(player.dormacyInfo, 1.0f))));
	}

	if (cfgflags.at(E2T(EspFlags::ZOOM)))
	{
		if (player.entity->m_bIsScoped())
			flags.emplace_back(std::make_pair("ZOOM", Colors::White.getColorEditAlpha(getDormantAlpha(player.dormacyInfo, 1.0f))));
	}

	if (cfgflags.at(E2T(EspFlags::C4)))
	{
		if (player.entity->isC4Owner())
			flags.emplace_back(std::make_pair("C4", Colors::Orange.getColorEditAlpha(getDormantAlpha(player.dormacyInfo, 1.0f))));
	}

	const float fontSize = game::getScaledFont(player.entity->absOrigin(), game::localPlayer()->absOrigin(), 60.0f, 11.0f, 16.0f);

	float paddingText = 0.0f;
	for (size_t i = 0; const auto & [name, color] : flags)
	{
		ImRender::text(player.box.x + player.box.w + padding.right + 2.0f, player.box.y + paddingText, fontSize,
			ImRender::fonts::verdana12, name, false, color, false);
		const auto textSize = ImRender::fonts::verdana12->CalcTextSizeA(fontSize, std::numeric_limits<float>::max(), 0.0f, name.c_str());
		paddingText += textSize.y;

		i++;

		if (i != flags.size() && paddingText + fontSize > player.box.h) // when too many flags for long distances
		{
			ImRender::text(player.box.x + player.box.w + padding.right + 2.0f, player.box.y + paddingText, fontSize, ImRender::fonts::verdana12,
				std::format("{} more...", flags.size() - i), false, Colors::White.getColorEditAlpha(getDormantAlpha(player.dormacyInfo, 1.0f)), false);
			break;
		}
	}

	// I don't handle padding here, because flags should be last to display
}

void playerVisuals::drawSnapLine(VisualEntity& player)
{
	if (!vars::visuals->esp->snapline->enabled)
		return;

	ImRender::drawLine(globals::screenX / 2.0f, 
		static_cast<float>(globals::screenY), player.box.x + player.box.w / 2, player.box.y + player.box.h, vars::visuals->esp->snapline->color());
}

void playerVisuals::drawLaser(VisualEntity& player)
{
	if (!vars::visuals->esp->lasers->enabled)
		return;

	// get from where to start, "laser ESP" is always starting from head I think
	const auto start = player.entity->getBonePos(8);
	// get angle to draw with correct view
	const auto forward = math::angleVec(player.entity->m_angEyeAngles());
	// end is where lines just ends, this 70 is hardcoded, but whatever here tbh
	constexpr float okayishDist{ 70.f };
	const auto end = start + forward * okayishDist;

	if (ImVec2 startP, endLine; ImRender::worldToScreen(start, startP) && ImRender::worldToScreen(end, endLine))
	{
		ImRender::drawCircleFilled(startP.x, startP.y, 3, 32, Colors::Red);
		ImRender::drawLine(startP, endLine, vars::visuals->esp->lasers->color().getColorEditAlpha(getDormantAlpha(player.dormacyInfo, vars::visuals->esp->lasers->color().a())));
	}
}

void playerVisuals::updateDormacy(VisualEntity& player)
{
	auto& dormacy = player.dormacyInfo;

	const float ratio = 1.0f / vars::visuals->dormacy->time;
	const float step = ratio * memory::interfaces::globalVars->m_frametime;

	if (player.entity->isDormant())
	{
		dormacy.alpha -= step;
		player.entity->setAbsOrigin(player.entity->absOrigin());
	}
	else
	{
		dormacy.lastPos = player.entity->absOrigin();
		dormacy.alpha += step;
		dormacy.lastUpdate = memory::interfaces::globalVars->m_curtime;
	}

	dormacy.alpha = std::clamp(dormacy.alpha, 0.0f, 1.0f);
}

void playerVisuals::roundRestart()
{
	for (auto& player : players)
		player.dormacyInfo.reset = true;
}