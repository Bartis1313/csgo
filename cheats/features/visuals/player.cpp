#include "player.hpp"

#include "../../../SDK/CGlobalVars.hpp"
#include "../../../SDK/IClientEntityList.hpp"
#include "../../../SDK/IVModelInfo.hpp"
#include "../../../SDK/IVEngineClient.hpp"
#include "../../../SDK/IVEffects.hpp"
#include "../../../SDK/IGameEvent.hpp"
#include "../../../SDK/IViewRenderBeams.hpp"
#include "../../../SDK/animations.hpp"
#include "../../../SDK/ICvar.hpp"
#include "../../../SDK/ConVar.hpp"
#include "../../../SDK/IWeapon.hpp"
#include "../../../SDK/structs/Entity.hpp"
#include "../../../SDK/Color.hpp"
#include "../../../SDK/interfaces/interfaces.hpp"

#include "../../../config/vars.hpp"
#include "../../game.hpp"
#include "../../../utilities/renderer/renderer.hpp"
#include "../../../utilities/math/math.hpp"
#include "../../globals.hpp"

#include "../../features/aimbot/aimbot.hpp"
#include "../../features/backtrack/backtrack.hpp"

void Visuals::run()
{
	if (!game::isAvailable())
		return;

	for (int i = 1; i <= interfaces::globalVars->m_maxClients; i++)
	{
		auto entity = reinterpret_cast<Player_t*>(interfaces::entList->getClientEntity(i));

		if (!entity)
			continue;

		if (game::localPlayer == entity)
			continue;

		if (entity->isDormant()) // TODO: put fading
			continue;

		if (!entity->isAlive())
			continue;

		if (entity->m_iTeamNum() == game::localPlayer->m_iTeamNum())
			continue;

		if (!config.get<bool>(vars.bEsp))
			return;

		auto runFeatures = [=]()
		{
			drawPlayer(entity);
			drawSkeleton(entity);
			runDLight(entity);
			drawLaser(entity);
		};

		if (config.get<bool>(vars.bDrawDeadOnly))
		{
			if (!game::localPlayer->isAlive())
				runFeatures();
		}
		else
			runFeatures();

		enemyIsAimingAtYou(entity);
	}
}

void Visuals::drawHealth(Player_t* ent, const Box& box)
{
	if (!config.get<bool>(vars.bDrawHealth))
		return;

	auto health = ent->m_iHealth() > 100 ? 100 : ent->m_iHealth();
	if (health)
	{
		auto offset = health * box.h / 100;
		auto pad = box.h - offset;

		Box newBox =
		{
			box.x - 6,
			box.y - 1,
			2,
			box.h + 2,
		};

		// fill first
		imRender.drawRectFilled(newBox.x, newBox.y, newBox.w, newBox.h, Colors::Black);
		imRender.drawRectFilled(newBox.x, newBox.y + pad - 1, 2, offset + 2, healthBased(ent));
		imRender.drawRect(newBox.x - 1, newBox.y - 1, 4, newBox.h, Colors::Black);

		// if the player has health below max, then draw HP info
		if (health < 100)
			imRender.text(newBox.x - 2, newBox.y + pad - 4, ImFonts::espBar, std::to_string(health), false, Colors::White);
	}
}

void Visuals::drawArmor(Player_t* ent, const Box& box)
{
	if(!config.get<bool>(vars.bDrawArmor))
		return;

	auto armor = ent->m_ArmorValue() > 100 ? 100 : ent->m_ArmorValue();
	if (armor)
	{
		auto offset = armor * box.h / 100;
		auto pad = box.h - offset;

		Box newBox =
		{
			box.x + box.w + 4,
			box.y - 1,
			2,
			box.h + 2,
		};

		Color armorCol = Color(0, static_cast<int>(armor * 1.4f), 250, 255); // light to blue, something simple

		imRender.drawRectFilled(newBox.x, newBox.y, newBox.w, newBox.h, Colors::Black);
		imRender.drawRectFilled(newBox.x, newBox.y + pad - 1, 2, offset + 2, armorCol);
		imRender.drawRect(newBox.x - 1, newBox.y - 1, 4, newBox.h, Colors::Black);


		if (armor < 100)
			imRender.text(newBox.x - 2, newBox.y + pad - 4, ImFonts::espBar, std::to_string(armor), false, Colors::White);
	}
}

void Visuals::drawWeapon(Player_t* ent, const Box& box)
{
	if (!config.get<bool>(vars.bDrawWeapon))
		return;

	auto weapon = ent->getActiveWeapon();
	if (!weapon)
		return;

	Color tex = config.get<Color>(vars.cWeaponText);

	imRender.text(box.x + box.w / 2, box.y + box.h + 5, ImFonts::espBar, ent->getActiveWeapon()->getWpnName(), true, tex);

	// skip useless trash for calculations
	if (weapon->isNonAimable())
		return;

	int maxAmmo = weapon->getWpnInfo()->m_maxClip1;
	int currentAmmo = weapon->m_iClip1();

	Box newBox =
	{
		box.x,
		box.y + box.h + 3,
		box.w + 2,
		2
	};

	int barWidth = currentAmmo * box.w / maxAmmo;
	bool isReloading = false;
	auto animlayer = ent->getAnimOverlays()[1];

	if (animlayer.m_sequence)
	{
		auto sequenceActivity = ent->getSequenceActivity(animlayer.m_sequence);
		isReloading = sequenceActivity == 967 && animlayer.m_weight; // ACT_CSGO_RELOAD

		if (isReloading && animlayer.m_weight != 0.0f && animlayer.m_cycle < 0.99f)
			barWidth = (animlayer.m_cycle * box.w) / 1.0f;
	}

	imRender.drawRectFilled(newBox.x - 1, newBox.y - 1, newBox.w, 4, Colors::Black);
	imRender.drawRectFilled(newBox.x, newBox.y, barWidth, 2, config.get<Color>(vars.cReloadbar));
	
	if (maxAmmo != currentAmmo && !isReloading)
		imRender.text(newBox.x + barWidth, newBox.y + 1, ImFonts::espBar, std::to_string(currentAmmo), false, tex);
}

void Visuals::drawInfo(Player_t* ent, const Box& box)
{
	if (!config.get<bool>(vars.bDrawInfos))
		return;

	imRender.text(box.x + (box.w / 2), box.y - 15, ImFonts::tahoma, ent->getName(), true, healthBased(ent));

	if (ent->isC4Owner())
		imRender.text(box.x - 25 + box.w, box.y + 5 + box.h, ImFonts::tahoma, XOR("C4"), false, healthBased(ent));
}

// yoinked: https://www.unknowncheats.me/wiki/Counter_Strike_Global_Offensive:Bone_ESP
void Visuals::drawSkeleton(Player_t* ent)
{
	if (!config.get<bool>(vars.bDrawSkeleton))
		return;

	auto model = ent->getModel();
	if (!model)
		return;

	auto studio = interfaces::modelInfo->getStudioModel(model);
	if (!studio)
		return;

	// have to check if selected record is filled, if no then just skip
	auto record = !backtrack.getAllRecords().at(ent->getIndex()).empty()  ? &backtrack.getAllRecords().at(ent->getIndex()) : nullptr;
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

	for (int i = 0; i < studio->m_bonesCount; i++)
	{
		auto bone = studio->bone(i);
		if (!bone)
			continue;

		if (bone->m_parent == -1)
			continue;

		if (!(bone->m_flags & BONE_USED_BY_HITBOX))
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

		if (std::abs(deltachild.z) < 5.0f && deltaparent.length() < 5.0f && deltachild.length() < 5.0f || i == chest)
			continue;

		if (Vector2D start, end; imRender.worldToScreen(parent, start) && imRender.worldToScreen(child, end))
			imRender.drawLine(start, end, config.get<Color>(vars.cSkeleton));
	}
}

void Visuals::drawSnapLine(Player_t* ent, const Box& box)
{
	if (ent == aimbot.getTargetted())
	{
		// lines on the bottom and center bottom box
		imRender.drawLine(globals::screenX / 2, globals::screenY, box.x + box.w / 2, box.y + box.h, Colors::Purple);
	}
}

void Visuals::drawLaser(Player_t* ent)
{
	if (!config.get<bool>(vars.bEspLasers))
		return;

	// get from where to start, "laser ESP" is always starting from head I think
	auto start = ent->getBonePos(8);
	// get angle to draw with correct view
	auto forward = math::angleVec(ent->m_angEyeAngles());
	// end is where lines just ends, this 70 is hardcoded, but whatever here tbh
	auto end = start + forward * 70.f;

	if (Vector screenLocal, screenEnt; imRender.worldToScreen(start, screenLocal) && imRender.worldToScreen(end, screenEnt))
	{
		imRender.drawCircleFilled(screenLocal.x, screenLocal.y, 3, 32, Colors::Red);
		imRender.drawLine(screenLocal.x, screenLocal.y, screenEnt.x, screenEnt.y, Colors::Purple);
	}
}

void Visuals::runDLight(Player_t* ent)
{
	// https://github.com/ValveSoftware/source-sdk-2013/blob/0d8dceea4310fde5706b3ce1c70609d72a38efdf/sp/src/game/client/c_spotlight_end.cpp#L112

	if (!config.get<bool>(vars.bDLight))
		return;

	auto DLight = interfaces::efx->clAllocDlight(ent->getIndex());
	DLight->m_style = DLIGHT_NO_WORLD_ILLUMINATION;
	DLight->m_color = { config.get<Color>(vars.cDlight) };
	DLight->m_origin = ent->absOrigin();
	DLight->m_radius = 80.0f;
	DLight->m_die = interfaces::globalVars->m_curtime + 0.05f;
	DLight->m_exponent = true;
	DLight->m_decay = 30.0f;
	DLight->m_key = ent->getIndex();
}

void Visuals::drawPlayer(Player_t* ent)
{
	if (!config.get<bool>(vars.bEsp))
		return;

	Box box; Box3D box3d;
	if (!utilities::getBox(ent, box, box3d))
		return;

	switch (config.get<int>(vars.iEsp))
	{
	case E2T(BoxTypes::BOX2D):
		drawBox2D(box);
		break;
	case E2T(BoxTypes::FILLED2D):
		drawBox2DFilled(box);
		break;
	case E2T(BoxTypes::BOX3D):
		drawBox3D(box3d);
		break;
	case E2T(BoxTypes::FILLED3D):
		drawBox3DFilled(box3d);
		break;
	default:
		break;
	}

	drawHealth(ent, box);
	drawArmor(ent, box);
	drawWeapon(ent, box);
	drawInfo(ent, box);
	drawSnapLine(ent, box);
}

// add this to events manager 
void Visuals::drawSound(IGameEvent* event)
{
	if (!config.get<bool>(vars.bSoundEsp))
		return;

	auto userid = interfaces::engine->getPlayerID(event->getInt(XOR("userid")));

	auto entity = reinterpret_cast<Player_t*>(interfaces::entList->getClientEntity(userid));

	if (!entity)
		return;

	if (!entity->isPlayer())
		return;

	if (!entity->isAlive())
		return;

	if (entity->m_bGunGameImmunity())
		return;

	static auto modelIndex = interfaces::modelInfo->getModelIndex(XOR("sprites/physbeam.vmt"));

	BeamInfo_t info = {};

	info.m_type = TE_BEAMRINGPOINT;
	info.m_flags = FBEAM_FADEOUT;
	info.m_modelName = XOR("sprites/physbeam.vmt");
	info.m_modelIndex = modelIndex;
	info.m_haloIndex = -1;
	info.m_haloScale = 3.0f;
	info.m_life = 2.0f;
	info.m_width = 4.0f;
	info.m_fadeLength = 1.0f;
	info.m_amplitude = 0.0f;
	info.m_red = 70.f;
	info.m_green = 130.f;
	info.m_blue = 200.f;
	info.m_brightness = 255.f;
	info.m_speed = 0.0f;
	info.m_startFrame = 0.0f;
	info.m_frameRate = 60.0f;	
	info.m_vecCenter = entity->absOrigin() + Vector(0.0f, 0.0f, 5.0f);
	info.m_startRadius = 5.0f;
	info.m_endRadius = 20.f;
	info.m_renderable = true;

	if (auto beamDraw = interfaces::beams->createRingPoint(info); beamDraw)
		interfaces::beams->drawBeam(beamDraw);
}

Color Visuals::healthBased(Player_t* ent)
{
	int health = ent->m_iHealth();
	int g = health * 2.55f;
	int r = 255 - g;
	return Color(r, g, 0, 255);
}

void Visuals::enemyIsAimingAtYou(Player_t* ent)
{
	if (!config.get<bool>(vars.bAimingWarn))
		return;

	if (!game::localPlayer)
		return;

	if (!interfaces::engine->isInGame())
		return;

	Vector posDelta = ent->getEyePos() - game::localPlayer->getEyePos();
	Vector idealAimAngle = math::vectorToAngle(posDelta);

	// account for their spray control
	static const auto scale = interfaces::cvar->findVar(XOR("weapon_recoil_scale"))->getFloat();
	idealAimAngle -= ent->m_aimPunchAngle() * scale;

	Vector curEnemyAngle = ent->m_angEyeAngles();
	curEnemyAngle.normalize();

	// check if we can even see the enemy, this might not work always
	// assume we have a fov changer, and enemy does not. This then is not valid
	auto isInScreenRange = [=]()
	{
		if (Vector2D s; imRender.worldToScreen(ent->absOrigin(), s))
		{
			bool xLimit = globals::screenX > s.x;
			bool yLimit = globals::screenY > s.y;

			return xLimit && yLimit;
		}

		return false;
	};
	// check trace
	bool check = ent->isPossibleToSee(game::localPlayer->getEyePos());

	// dynamic fov
	float fov = math::calcFovReal(ent->getEyePos(), game::localPlayer->getBonePos(3), curEnemyAngle); // 3 is middle body

	if (check && isInScreenRange())
	{
		imRender.text(globals::screenX / 2, 60, ImFonts::tahoma, XOR("Enemy can see you"), true, Colors::Green);
	}
	// this can be made with tracing
	if (fov <= 5.0f)
	{
		imRender.text(globals::screenX / 2, 80, ImFonts::tahoma, XOR("Enemy is aiming you"), true, Colors::Red);
	}
}

#pragma region drawing_boxes

void Visuals::drawBox2D(const Box& box)
{
	Color cfgCol = config.get<Color>(vars.cBox);

	imRender.drawRect(box.x - 1.0f, box.y - 1.0f, box.w + 2.0f, box.h + 2.0f, Color(0, 0, 0, 200));
	imRender.drawRect(box.x + 1.0f, box.y + 1.0f, box.w - 2.0f, box.h - 2.0f, Color(0, 0, 0, 200));
	imRender.drawRect(box.x, box.y, box.w, box.h, cfgCol);
}

void Visuals::drawBox2DFilled(const Box& box)
{
	Color fill = config.get<Color>(vars.cBoxFill);

	imRender.drawRectFilled(box.x + 1.0f, box.y + 1.0f, box.w - 2.0f, box.h - 2.0f, fill);
	drawBox2D(box);
}

void Visuals::drawBox3DFilled(const Box3D& box, const float thickness)
{
	Color color = config.get<Color>(vars.cBox);
	Color fill = config.get<Color>(vars.cBoxFill);

	imRender.drawQuadFilled(box.points.at(0), box.points.at(1), box.points.at(2), box.points.at(3), fill);
	// top
	imRender.drawQuadFilled(box.points.at(4), box.points.at(5), box.points.at(6), box.points.at(7), fill);
	// front
	imRender.drawQuadFilled(box.points.at(3), box.points.at(2), box.points.at(6), box.points.at(7), fill);
	// back
	imRender.drawQuadFilled(box.points.at(0), box.points.at(1), box.points.at(5), box.points.at(4), fill);
	// right
	imRender.drawQuadFilled(box.points.at(2), box.points.at(1), box.points.at(5), box.points.at(6), fill);
	// left
	imRender.drawQuadFilled(box.points.at(3), box.points.at(0), box.points.at(4), box.points.at(7), fill);

	for (size_t i = 0; i < 3; i++)
	{
		imRender.drawLine(box.points.at(i), box.points.at(i + 1), color);
	}
	// missing part at the bottom
	imRender.drawLine(box.points.at(0), box.points.at(3), color);
	// top parts
	for (size_t i = 4; i < 7; i++)
	{
		imRender.drawLine(box.points.at(i), box.points.at(i + 1), color);
	}
	// missing part at the top
	imRender.drawLine(box.points.at(4), box.points.at(7), color);
	// now all 4 box.points missing parts for 3d box
	for (size_t i = 0; i < 4; i++)
	{
		imRender.drawLine(box.points.at(i), box.points.at(i + 4), color);
	}
}

void Visuals::drawBox3D(const Box3D& box, const float thickness)
{
	Color color = config.get<Color>(vars.cBox);

	for (size_t i = 0; i < 3; i++)
	{
		imRender.drawLine(box.points.at(i), box.points.at(i + 1), color);
	}
	// missing part at the bottom
	imRender.drawLine(box.points.at(0), box.points.at(3), color);
	// top parts
	for (size_t i = 4; i < 7; i++)
	{
		imRender.drawLine(box.points.at(i), box.points.at(i + 1), color);
	}
	// missing part at the top
	imRender.drawLine(box.points.at(4), box.points.at(7), color);
	// now all 4 box.points missing parts for 3d box
	for (size_t i = 0; i < 4; i++)
	{
		imRender.drawLine(box.points.at(i), box.points.at(i + 4), color);
	}
}

#pragma endregion