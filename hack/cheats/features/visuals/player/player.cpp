#include "player.hpp"

#include "boxes.hpp"
#include "enemyWarn.hpp"
#include "sounds.hpp"
#include "../../aimbot/aimbot.hpp"
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
#include <SDK/structs/Entity.hpp>
#include <SDK/interfaces/interfaces.hpp>
#include <gamememory/memory.hpp>
#include <config/vars.hpp>
#include <cheats/game/game.hpp>
#include <render/render.hpp>
#include <utilities/math/math.hpp>
#include <cheats/game/globals.hpp>

#include <ranges>

void PlayerVisuals::init()
{
	events::add(XOR("round_prestart"), std::bind(&PlayerVisuals::resetDormacy, this, std::placeholders::_1));
}

void PlayerVisuals::draw()
{
	if (!vars::visuals->esp->boxes->enabled)
		return;

	if (!game::isAvailable())
		return;

	bool drawDead = vars::visuals->esp->checks->dead;

	std::pair<bool, bool> warnChecks = { false, false };

	bool isFlashOk = true;
	if (game::localPlayer->m_flFlashDuration() > 0.0f)
	{
		if (game::localPlayer->m_flFlashBangTime() >= vars::visuals->esp->checks->flashLimit)
			isFlashOk = false;
	}

	for (auto [entity, idx, classID] : EntityCache::getCache(EntCacheType::PLAYER))
	{
		auto ent = reinterpret_cast<Player_t*>(entity);

		if (ent == game::localPlayer)
			continue;

		if (!ent->isAlive())
			continue;

		if (!ent->isOtherTeam(game::localPlayer()))
			continue;

		updateDormacy(ent);

		if (m_calledEvent)
			continue;

		if (!m_dormant.at(idx).isValid())
			continue;

		// not going to render targets that are too far away
		if (game::localPlayer->absOrigin().distToMeters(entity->absOrigin()) > 150.0f)
			continue;

		auto runFeatures = [=]()
		{
			if (vars::visuals->esp->checks->visible
				&& !game::localPlayer->isPossibleToSee(ent, ent->getHitboxPos(HITBOX_HEAD)))
				return;

			if (vars::visuals->esp->checks->visible
				&& game::localPlayer->isViewInSmoke(ent->getHitboxPos(HITBOX_BELLY)))
				return;

			if (!isFlashOk)
				return;

			drawPlayer(ent);
			drawSkeleton(ent);
			runDLight(ent);
			drawLaser(ent);
			SoundDraw::findBest(ent);
		};

		if (drawDead)
		{
			if (!game::localPlayer->isAlive())
				runFeatures();
		}
		else
			runFeatures();

		warnChecks = g_EnemyWarning->check(ent);
	}
	SoundDraw::draw();
	g_EnemyWarning->draw(warnChecks);
}

void PlayerVisuals::drawHealth(Player_t* ent, const Box& box)
{
	if (!vars::visuals->esp->healthBar->enabled)
		return;

	constexpr int maxHealth = 100;
	constexpr float frequency = maxHealth * (1.0f / 0.5f);

	auto& health = m_health.at(ent->getIndex());
	const auto realHealth = ent->m_iHealth();
	if (health > realHealth)
		health -= frequency * memory::interfaces::globalVars->m_frametime;
	else
		health = static_cast<float>(realHealth);

	const auto offset = health * box.h / maxHealth;
	const auto pad = box.h - offset;

	Box newBox =
	{
		box.x - 5.0f,
		box.y,
		2.0f,
		box.h,
	};

	// fill first
	imRender.drawRoundedRectFilled(newBox.x - 1.0f, newBox.y - 1.0f, 4.0f, newBox.h + 2.0f, 120.0f, Colors::Black.getColorEditAlpha(m_dormant.at(ent->getIndex()).m_alpha));
	imRender.drawRoundedRectFilled(newBox.x, newBox.y + pad, 2.0f, offset, 120.0f, Color::healthBased(ent->m_iHealth()).getColorEditAlpha(m_dormant.at(ent->getIndex()).m_alpha));

	// if the player has health below max, then draw HP info
	if (health < 100)
	{
		auto text = FORMAT(XOR("{}"), realHealth);
		float fontSize = game::getScaledFont(ent->absOrigin(), game::localPlayer->absOrigin(), 100.0f, 10.0f, 14.0f);
		auto size = ImFonts::franklinGothic12->CalcTextSizeA(fontSize, std::numeric_limits<float>::max(), 0.0f, text.c_str());

		imRender.text(newBox.x - 4.0f - size.x, newBox.y + pad - 4.0f,
			fontSize, ImFonts::franklinGothic12, text, false, Colors::White.getColorEditAlpha(m_dormant.at(ent->getIndex()).m_alpha));
	}
}

void PlayerVisuals::drawArmor(Player_t* ent, const Box& box)
{
	if (!vars::visuals->esp->armorBar->enabled)
		return;
	
	constexpr int maxArmor = 100;
	constexpr float frequency = maxArmor * (1.0f / 0.5f);
	auto& armor = m_armor.at(ent->getIndex());
	const auto realArmor = ent->m_ArmorValue();

	if (armor > realArmor)
		armor -= frequency * memory::interfaces::globalVars->m_frametime;
	else
		armor = static_cast<float>(realArmor);

	const auto offset = armor * box.h / maxArmor;
	const auto pad = box.h - offset;

	Box newBox =
	{
		box.x + box.w + 3.0f,
		box.y,
		2.0f,
		box.h,
	};

	Color armorCol = Color(0, static_cast<int>(armor * 1.4f), 255); // light to blue, something simple

	if (armor != 0)
	{
		imRender.drawRoundedRectFilled(newBox.x - 1.0f, newBox.y - 1.0f, 4.0f, newBox.h + 2.0f, 120.0f, Colors::Black.getColorEditAlpha(m_dormant.at(ent->getIndex()).m_alpha));
		imRender.drawRoundedRectFilled(newBox.x, newBox.y + pad, 2.0f, offset, 120.0f, armorCol.getColorEditAlpha(m_dormant.at(ent->getIndex()).m_alpha));
	}

	/*if (armor < 100 && armor != 0)
		imRender.text(newBox.x - 2.0f, newBox.y + pad - 4.0f,
			ImFonts::franklinGothic, std::format(XOR("{}"), realArmor), false, Colors::White);*/
}

#include <SDK/ILocalize.hpp>

void PlayerVisuals::drawWeapon(Player_t* ent, const Box& box)
{
	if (!vars::visuals->esp->weaponBar->enabled)
		return;

	auto weapon = ent->getActiveWeapon();
	if (!weapon)
		return;

	Color tex = vars::visuals->esp->weaponBar->text();

	int maxAmmo = weapon->getWpnInfo()->m_maxClip1;
	int currentAmmo = weapon->m_iClip1();

	imRender.text(box.x + box.w / 2, box.y + box.h + 5, ImFonts::franklinGothic12, FORMAT(XOR("{} {}/{}"),
		vars::visuals->esp->weaponBar->translate ? memory::interfaces::localize->findAsUTF8(weapon->getWpnInfo()->m_WeaponName) : ent->getActiveWeapon()->getWpnName(), currentAmmo, maxAmmo),
		true, tex.getColorEditAlpha(m_dormant.at(ent->getIndex()).m_alpha));

	// skip useless trash for calculations
	if (weapon->isNonAimable())
		return;

	Box newBox =
	{
		box.x,
		box.y + box.h + 3.0f,
		box.w + 2.0f,
		2.0f
	};

	float barWidth = currentAmmo * box.w / maxAmmo;
	bool isReloading = false;
	auto animlayer = ent->getAnimOverlays()[1];

	if (animlayer.m_sequence)
	{
		auto sequenceActivity = ent->getSequenceActivity(animlayer.m_sequence);
		isReloading = sequenceActivity == 967 && animlayer.m_weight; // ACT_CSGO_RELOAD

		if (isReloading && animlayer.m_weight != 0.0f && animlayer.m_cycle < 0.99f)
			barWidth = (animlayer.m_cycle * box.w) / 1.0f;
	}

	imRender.drawRectFilled(newBox.x - 1.0f, newBox.y - 1.0f, newBox.w, 4.0f, Colors::Black.getColorEditAlpha(m_dormant.at(ent->getIndex()).m_alpha));
	imRender.drawRectFilled(newBox.x, newBox.y, barWidth, 2.0f, vars::visuals->esp->weaponBar->bar().getColorEditAlpha(m_dormant.at(ent->getIndex()).m_alpha));
}

void PlayerVisuals::drawInfo(Player_t* ent, const Box& box)
{
	std::vector<std::pair<std::string, Color>> flags = {};
	using cont = std::vector<bool>; // container

	PlayerInfo_t info = {};
	if (!memory::interfaces::engine->getPlayerInfo(ent->getIndex(), &info))
		return;

	auto cfgflags = vars::visuals->esp->flags->flags;

	if (cfgflags.at(E2T(EspFlags::BOT)))
		if(info.m_fakePlayer)
			flags.emplace_back(std::make_pair(XOR("BOT"), Colors::Yellow.getColorEditAlpha(m_dormant.at(ent->getIndex()).m_alpha)));

	if (cfgflags.at(E2T(EspFlags::MONEY)))
		flags.emplace_back(std::make_pair(FORMAT(XOR("{}$"), ent->m_iAccount()), Colors::Green.getColorEditAlpha(m_dormant.at(ent->getIndex()).m_alpha)));

	if (cfgflags.at(E2T(EspFlags::WINS)))
		flags.emplace_back(std::make_pair(FORMAT(XOR("Wins {}"), ent->getWins()), Colors::Green.getColorEditAlpha(m_dormant.at(ent->getIndex()).m_alpha)));

	if (cfgflags.at(E2T(EspFlags::RANK)))
		flags.emplace_back(std::make_pair(FORMAT(XOR("Rank {}"), ent->getRank()), Colors::White.getColorEditAlpha(m_dormant.at(ent->getIndex()).m_alpha)));

	if (cfgflags.at(E2T(EspFlags::ARMOR)))
	{
		std::string text = "";
		if (ent->m_bHasHelmet() && ent->m_ArmorValue())
			text = XOR("H KEV");
		else if (!ent->m_bHasHelmet() && ent->m_ArmorValue())
			text = XOR("KEV");

		flags.emplace_back(std::make_pair(text, Colors::White));
	}

	if (cfgflags.at(E2T(EspFlags::ZOOM)))
		if(ent->m_bIsScoped())
			flags.emplace_back(std::make_pair(XOR("ZOOM"), Colors::White.getColorEditAlpha(m_dormant.at(ent->getIndex()).m_alpha)));

	if (cfgflags.at(E2T(EspFlags::C4)))
		if (ent->isC4Owner())
			flags.emplace_back(std::make_pair(XOR("C4"), Colors::Orange.getColorEditAlpha(m_dormant.at(ent->getIndex()).m_alpha)));

	float fontSize = game::getScaledFont(ent->absOrigin(), game::localPlayer()->absOrigin(), 60.0f, 11.0f, 16.0f);

	float padding = 0.0f;
	float addon = vars::visuals->esp->armorBar->enabled ? 6.0f : 0.0f;

	for (size_t i = 0; const auto & [name, color] : flags)
	{
		imRender.text(box.x + box.w + addon + 2.0f, box.y + padding, fontSize, ImFonts::verdana12, name, false, color, false);
		auto textSize = ImFonts::verdana12->CalcTextSizeA(fontSize, std::numeric_limits<float>::max(), 0.0f, name.c_str());
		padding += textSize.y;
		i++;

		if (i != flags.size() && padding + fontSize > box.h) // when too many flags for long distances
		{
			imRender.text(box.x + box.w + addon + 2.0f, box.y + padding, fontSize, ImFonts::verdana12,
				FORMAT(XOR("{} more..."), flags.size() - i), false, Colors::White.getColorEditAlpha(m_dormant.at(ent->getIndex()).m_alpha), false);
			break;
		}
	}
}

void PlayerVisuals::drawnName(Player_t* ent, const Box& box)
{
	if (!vars::visuals->esp->nameBar->enabled)
		return;

	float fontSize = game::getScaledFont(ent->absOrigin(), game::localPlayer()->absOrigin());

	imRender.text(box.x + box.w / 2.0f, box.y - fontSize - 2.0f, fontSize, ImFonts::verdana12, ent->getName(), true,
		Color::healthBased(ent->m_iHealth()).getColorEditAlpha(m_dormant.at(ent->getIndex()).m_alpha), false);
}

// yoinked: https://www.unknowncheats.me/wiki/Counter_Strike_Global_Offensive:Bone_ESP
void PlayerVisuals::drawSkeleton(Player_t* ent)
{
	if (!vars::visuals->esp->skeleton->enabled)
		return;

	auto model = ent->getModel();
	if (!model)
		return;

	auto studio = memory::interfaces::modelInfo->getStudioModel(model);
	if (!studio)
		return;

	// have to check if selected record is filled, if no then just skip
	auto record = !g_Backtrack->getAllRecords().at(ent->getIndex()).empty() ? &g_Backtrack->getAllRecords().at(ent->getIndex()) : nullptr;
	auto skeletPos = [=](const size_t idx)
	{
		auto child = record != nullptr
			? record->back().m_matrix[idx].origin()
			: ent->getBonePos(idx);
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

		if (record && !g_Backtrack->isValid(record->front().m_simtime)) // if backtrack
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
			if (ImVec2 s; imRender.worldToScreen(ent->getBonePos(i), s))
				imRender.text(s.x, s.y, ImFonts::franklinGothic12, FORMAT(XOR("{}"), i), true, Colors::White, true);
		}

		if (ImVec2 start, end; imRender.worldToScreen(parent, start) && imRender.worldToScreen(child, end))
			imRender.drawLine(start, end, vars::visuals->esp->skeleton->color().getColorEditAlpha(m_dormant.at(ent->getIndex()).m_alpha));
	}
}

void PlayerVisuals::drawSnapLine(Player_t* ent, const Box& box)
{
	if (ent == g_Aimbot->getTargetted())
	{
		// lines on the bottom and center bottom box
		imRender.drawLine(globals::screenX / 2.0f, static_cast<float>(globals::screenY), box.x + box.w / 2, box.y + box.h, Colors::Purple);
	}
}

void PlayerVisuals::drawLaser(Player_t* ent)
{
	if (!vars::visuals->esp->lasers->enabled)
		return;

	// get from where to start, "laser ESP" is always starting from head I think
	auto start = ent->getBonePos(8);
	// get angle to draw with correct view
	auto forward = math::angleVec(ent->m_angEyeAngles());
	// end is where lines just ends, this 70 is hardcoded, but whatever here tbh
	auto end = start + forward * 70.f;

	if (ImVec2 startP, endLine; imRender.worldToScreen(start, startP) && imRender.worldToScreen(end, endLine))
	{
		imRender.drawCircleFilled(startP.x, startP.y, 3, 32, Colors::Red);
		imRender.drawLine(startP, endLine, Colors::Purple.getColorEditAlpha(m_dormant.at(ent->getIndex()).m_alpha));
	}
}

void PlayerVisuals::runDLight(Player_t* ent)
{
	// https://github.com/ValveSoftware/source-sdk-2013/blob/0d8dceea4310fde5706b3ce1c70609d72a38efdf/sp/src/game/client/c_spotlight_end.cpp#L112

	if (!vars::visuals->esp->dlight->enabled)
		return;

	auto dLight = memory::interfaces::efx->clAllocDLight(ent->getIndex());
	dLight->m_color = vars::visuals->esp->dlight->color();
	dLight->m_origin = ent->m_vecOrigin();
	dLight->m_radius = vars::visuals->esp->dlight->radius;
	dLight->m_die = memory::interfaces::globalVars->m_curtime + 0.1f;
	dLight->m_exponent = static_cast<char>(vars::visuals->esp->dlight->exponent);
	dLight->m_decay = vars::visuals->esp->dlight->decay;
	dLight->m_key = ent->getIndex();
}

void PlayerVisuals::drawPlayer(Player_t* ent)
{
	if (!vars::visuals->esp->boxes->enabled)
		return;

	Box box{ent};
	if (!box.isValid())
		return;

	constexpr float maxDist = 100.0f; // start fade
	constexpr float closeLimit = 5.0f; // don't draw too close
	float dist = game::localPlayer->absOrigin().distToMeters(m_dormant.at(ent->getIndex()).m_lastPos);
	float ratio = (dist - closeLimit) / maxDist;
	ratio = std::clamp(ratio, 0.0f, 0.45f); // clamp it to lower alpha

	// if dormacy is fade upadting, then use this alpha
	if (auto dormacyA = m_dormant.at(ent->getIndex()).m_alpha; dormacyA > ratio)
		m_boxAlpha.at(ent->getIndex()) = dormacyA;
	else // if not, use distance fade logic
	{
		float ratioDormant = 1.0f / (1.0f / vars::visuals->dormacy->time);
		float step = ratioDormant * memory::interfaces::globalVars->m_frametime;

		m_boxAlpha.at(ent->getIndex()) += step;
		m_boxAlpha.at(ent->getIndex()) = std::clamp(m_boxAlpha.at(ent->getIndex()), 0.0f, ratio);
	}

	bool isDormant = ent->isDormant();

	switch (vars::visuals->esp->boxes->mode)
	{
	case E2T(BoxTypes::BOX2D):
		BoxesDraw::drawBox2D(box, isDormant, m_boxAlpha.at(ent->getIndex()));
		break;
	case E2T(BoxTypes::FILLED2D):
		BoxesDraw::drawBox2DFilled(box, isDormant, m_boxAlpha.at(ent->getIndex()));
		break;
	case E2T(BoxTypes::BOX3D):
		BoxesDraw::drawBox3D(box, isDormant, m_boxAlpha.at(ent->getIndex()));
		break;
	case E2T(BoxTypes::FILLED3D):
	{
		if (!vars::visuals->esp->boxes->multiColor)
			BoxesDraw::drawBox3DFilled(box, isDormant, m_boxAlpha.at(ent->getIndex()));
		else
			BoxesDraw::drawBox3DFilledMultiColor(box, isDormant, m_boxAlpha.at(ent->getIndex()));
		break;
	}
	default:
		break;
	}

	drawHealth(ent, box);
	drawArmor(ent, box);
	drawWeapon(ent, box);
	drawInfo(ent, box);
	drawSnapLine(ent, box);
	drawnName(ent, box);
}

void PlayerVisuals::updateDormacy(Player_t* ent)
{
	auto& dormacy = m_dormant.at(ent->getIndex());

	float ratio = 1.0f / vars::visuals->dormacy->time;
	float step = ratio * memory::interfaces::globalVars->m_frametime;

	if (ent->isDormant())
	{
		dormacy.m_alpha -= step;
		ent->setAbsOrigin(ent->absOrigin());
	}
	else
	{
		m_calledEvent = false; // to remove problems on round restarts
		dormacy.m_lastPos = ent->absOrigin();
		dormacy.m_alpha += step;
		dormacy.m_lastUpdate = memory::interfaces::globalVars->m_curtime;
	}

	dormacy.m_alpha = std::clamp(dormacy.m_alpha, 0.0f, 1.0f);
}

bool PlayerVisuals::DormacyInfo_t::isValid() const
{
	return memory::interfaces::globalVars->m_curtime - m_lastUpdate < vars::visuals->dormacy->limit;
}

void PlayerVisuals::resetDormacy(IGameEvent* event)
{
	m_calledEvent = true;
}