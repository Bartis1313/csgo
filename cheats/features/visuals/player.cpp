#include <format>
#include "player.hpp"
#include "../../menu/vars.hpp"
#include "../../game.hpp"
#include "../../../utilities/renderer/renderer.hpp"
#include "../../../SDK/Enums.hpp"
#include "../../globals.hpp"
#include "../aimbot/aimbot.hpp"
#include "../backtrack/backtrack.hpp"

#undef min
#undef max

enum boxTypes
{
	OFF = 0,
	BOX2D,
	FILLED2D,
	BOX3D,
	FILLED3D
};

// very simple implementation of what to add as color
inline Color playerColor(Player_t* ent, bool isAimbotColor = false)
{
	if (legitbot::bestEnt == ent && isAimbotColor)
		return Colors::Purple;

	uint8_t health = ent->m_iHealth();
	uint8_t g = static_cast<uint8_t>(health * 2.55f);
	uint8_t r = 255 - g;
	return Color(
		r,
		g,
		0,
		220
	);
}

// dead esp - true = draw when dead, TODO

void esp::run()
{
	if (!game::localPlayer)
		return;

	for (int i = 1; i < interfaces::globalVars->m_maxClients; i++)
	{
		auto entity = reinterpret_cast<Player_t*>(interfaces::entList->getClientEntity(i));

		if (!entity)
			continue;

		if (game::localPlayer == entity)
			continue;

		if (entity->isDormant())
			continue;

		if (!entity->isAlive())
			continue;

		if (entity->m_iTeamNum() == game::localPlayer->m_iTeamNum())
			continue;

		// draw only when ent is valid, don't remove this if
		if (entity)
		{
			drawPlayer(entity);
			drawSkeleton(entity);
			runDLight(entity);
			drawLaser(entity);
		}
	}
}

void esp::renderBox3D(Entity_t* ent, bool fill)
{
	const auto col = ent->collideable();
	if (!col)
		return;

	const auto& min = col->OBBMins();
	const auto& max = col->OBBMaxs();

	std::array<Vector, 8> points =
	{
		Vector(min.x, min.y, min.z),
		Vector(min.x, max.y, min.z),
		Vector(max.x, max.y, min.z),
		Vector(max.x, min.y, min.z),

		Vector(min.x, min.y, max.z),
		Vector(min.x, max.y, max.z),
		Vector(max.x, max.y, max.z),
		Vector(max.x, min.y, max.z)
	};

	const auto& tranFrame = ent->m_rgflCoordinateFrame();

	std::array<Vector, 8> transformed = {};

	for (int i = 0; i < 8; i++)
		VectorTransform(points.at(i), tranFrame, transformed.at(i));
	
	render::drawBox3D(transformed, playerColor(reinterpret_cast<Player_t*>(ent), true), fill ? true : false);
}

void esp::drawBox2D(Player_t* ent, const Box& box)
{
	render::drawOutlineRect(box.x - 1, box.y - 1, box.w + 2, box.h + 2, Color(0, 0, 0,  200));
	render::drawOutlineRect(box.x, box.y, box.w, box.h, playerColor(ent, true));
	render::drawOutlineRect(box.x + 1, box.y + 1, box.w - 2, box.h - 2, Color(0, 0, 0, 200));
}

void esp::drawBox2DFilled(Player_t* ent, const Box& box)
{
	const uint8_t alphaOutline = static_cast<uint8_t>(255 * 0.6f);
	const Color fill{ 0, 0, 0, alphaOutline };

	// first create rectangle then do outlines

	render::drawFilledRect(box.x - 1, box.y - 1, box.w + 2, box.h + 2, fill);
	drawBox2D(ent, box);
}

void esp::drawHealth(Player_t* ent, const Box& box)
{
	if (!vars::bDrawInfos)
		return;

	auto health = ent->m_iHealth() > 100 ? 100 : ent->m_iHealth();
	if (health)
	{
		auto offset = health * box.h / 100;
		auto pad = box.h - offset;

		Box newBox =
		{
			box.x - 7,
			box.y - 1,
			4,
			box.h + 2,
		};

		// fill first
		render::drawFilledRect(newBox.x, newBox.y, newBox.w, newBox.h, Colors::Black);
		render::drawFilledRect(newBox.x + 1, newBox.y + pad + 1, 2, offset, playerColor(ent));
		// if the player has health below max, then draw HP info
		// also 12 val is pretty hardcoded here, I don't use calcsize function for such small texts
		if (health < 100)
			render::text(newBox.x - 16, newBox.y + 1 + pad + 8, fonts::tahoma, std::to_string(health), false, Colors::White);
	}
}

// same as health with how it works, but different pos only, only difference is small math with x,y 
// why not 6 and 7 here too? should be easy to answer, in graphics in this case I start from left side and end on right, use logic
void esp::drawArmor(Player_t* ent, const Box& box)
{
	if (!vars::bDrawInfos)
		return;

	auto armor = ent->m_ArmorValue() > 100 ? 100 : ent->m_ArmorValue();
	if (armor)
	{
		auto offset = armor * box.h / 100;
		auto pad = box.h - offset;

		Box newBox =
		{
			box.x + box.w + 2,
			box.y - 1,
			4,
			box.h + 2
		};

		render::drawFilledRect(newBox.x, newBox.y, newBox.w, newBox.h, Colors::Black);
		Color armorCol = Color(0, armor * 1.4f, 250, 255); // light to blue, something simple
		render::drawFilledRect(newBox.x + 1, newBox.y + pad + 1, 2, offset, armorCol);

		if (armor < 100)
			render::text(newBox.x + 2, newBox.y + 1 + pad + 12, fonts::tahoma, std::to_string(armor), false, Colors::White);
	}
}

// TODO: Smooth reload time, or even detect time of reload
void esp::drawWeapon(Player_t* ent, const Box& box)
{
	if (!vars::bDrawInfos)
		return;

	auto weapon = ent->getActiveWeapon();
	if (!weapon)
		return;

	render::text(box.x + box.w + 5, box.y - 1, fonts::tahoma, ent->getActiveWeapon()->getWpnName(), false, Colors::White);

	// skip useless trash for calculations
	if (weapon->isNonAimable())
		return;

	auto maxAmmo = weapon->getWpnInfo()->m_maxClip1;
	auto currentAmmo = weapon->m_iClip1();

	Box newBox =
	{
		box.x,
		box.y + box.h + 3,
		box.w + 2,
		2
	};

	auto offset = currentAmmo * box.w / maxAmmo;
	render::drawFilledRect(newBox.x - 1, newBox.y - 1, newBox.w, 4, Colors::Black);
	render::drawFilledRect(newBox.x, newBox.y, offset, 2, Colors::Turquoise);
	
	if (maxAmmo != currentAmmo)
		render::text(newBox.x + offset, newBox.y + 1, fonts::tahoma, std::to_string(currentAmmo), false, Colors::White);
}

void esp::drawInfo(Player_t* ent, const Box& box)
{
	if (!vars::bShowFlags)
		return;

	render::text(box.x + (box.w / 2), box.y - 15, fonts::tahoma, ent->getName(), true, playerColor(ent));

	if (ent->isC4Owner())
		render::text(box.x - 25 + box.w, box.y + 5 + box.h, fonts::tahoma, "C4", false, playerColor(ent));
}

// yoinked: https://www.unknowncheats.me/wiki/Counter_Strike_Global_Offensive:Bone_ESP
void esp::drawSkeleton(Player_t* ent)
{
	if (!vars::bDrawSkeleton)
		return;

	auto model = ent->getModel();
	if (!model)
		return;

	auto studio = interfaces::modelInfo->getStudioModel(model);
	if (!studio)
		return;

	// have to check if selected record is filled, if no then just skip
	auto record = !backtrack::records[ent->getIndex()].empty() ? &backtrack::records[ent->getIndex()] : nullptr;

	for (int i = 0; i < studio->m_bonesCount; i++)
	{
		auto bone = studio->bone(i);
		if (!bone)
			continue;

		if (bone->m_parent == -1)
			continue;

		if (!(bone->m_flags & BONE_USED_BY_HITBOX))
			continue;

		// skip like here
		auto child = record
			? record->back().matrix[i].origin()
			: ent->getBonePosition(i);

		auto parent = record
			? record->back().matrix[bone->m_parent].origin()
			: ent->getBonePosition(bone->m_parent);

		auto chest = 6;

		auto upper = record
			? record->back().matrix[chest + 1].origin() - record->back().matrix[chest].origin()
			: ent->getBonePosition(chest + 1) - ent->getBonePosition(chest);

		auto breast = record
			? record->back().matrix[chest].origin() + upper / 2
			: ent->getBonePosition(chest) + upper / 2;

		auto deltachild = child - breast;
		auto deltaparent = parent - breast;

		if (deltaparent.Length() < 9.0f && deltachild.Length() < 9.0f)
			parent = breast;

		if (i == 5)
			child = breast;

		if (abs(deltachild.z) < 5.0f && deltaparent.Length() < 5.0f && deltachild.Length() < 5.0f || i == 6)
			continue;

		Vector screenp, screenc;

		if (render::WorldToScreen(parent, screenp) && render::WorldToScreen(child, screenc))
		{
			if(record && backtrack::isValid(record->front().simTime))
				render::drawLine(screenp[0], screenp[1], screenc[0], screenc[1], Colors::White);
			else if(!record)
				render::drawLine(screenp[0], screenp[1], screenc[0], screenc[1], Colors::White);
		}
	}
}

void esp::drawSnapLine(Player_t* ent, const Box& box)
{
	if (ent == legitbot::bestEnt)
	{
		int width, height;
		interfaces::engine->getScreenSize(width, height);

		// lines on the bottom and center bottom box
		render::drawLine(width / 2, height, box.x + box.w / 2, box.y + box.h, Colors::Purple);
	}
}

void esp::drawLaser(Player_t* ent)
{
	if (!vars::bEspLasers)
		return;

	// get from where to start, "laser ESP" is always starting from head I think
	auto start = ent->getBonePosition(8);
	// get angle to draw with correct view
	auto forward = math::angleVec(ent->m_angEyeAngles());
	// end is where lines just ends, this 70 is hardcoded, but whatever here tbh
	auto end = start + forward * 70.f;

	if (Vector screenLocal, screenEnt; render::WorldToScreen(start, screenLocal) && render::WorldToScreen(end, screenEnt))
	{
		render::drawCircleFilled(screenLocal.x, screenLocal.y, 3, 32, Colors::Red);
		// was lazy to make a polygon for only 2 width
		render::drawLine(screenLocal.x, screenLocal.y, screenEnt.x, screenEnt.y, Color(190, 190, 190, 200));
		render::drawLine(screenLocal.x + 1, screenLocal.y + 1, screenEnt.x + 1, screenEnt.y + 1, Color(190, 190, 190, 200));
	}
}

void esp::runDLight(Player_t* ent)
{
	// https://github.com/ValveSoftware/source-sdk-2013/blob/0d8dceea4310fde5706b3ce1c70609d72a38efdf/sp/src/game/client/c_spotlight_end.cpp#L112

	if (!interfaces::engine->isInGame())
		return;

	if (!vars::bDLight)
		return;

	auto DLight = interfaces::effects->clAllocDlight(ent->getIndex());
	DLight->m_style = DLIGHT_NO_WORLD_ILLUMINATION;
	DLight->m_color = { 20, 70, 150 };
	DLight->m_origin = ent->absOrigin();
	DLight->m_radius = 80.0f;
	DLight->m_die = interfaces::globalVars->m_curtime + 0.05f;
	DLight->m_exponent = 10.0f;
	DLight->m_decay = 30.0f;
	DLight->m_key = ent->getIndex();
}

void esp::drawPlayer(Player_t* ent)
{
	Box box;
	if (!utilities::getBox(ent, box))
		return;

	switch (vars::iEsp)
	{
	case BOX2D:
		drawBox2D(ent, box);
		break;
	case FILLED2D:
		drawBox2DFilled(ent, box);
		break;
	case BOX3D:
		renderBox3D(ent, false);
		break;
	case FILLED3D:
		renderBox3D(ent, true);
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
void esp::drawSound(IGameEvent* event)
{
	if (!vars::bSoundEsp)
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

	auto modelIndex = interfaces::modelInfo->getModelIndex(XOR("sprites/physbeam.vmt"));

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

	auto beam_draw = interfaces::beams->createRingPoint(info);

	if (beam_draw)
		interfaces::beams->drawBeam(beam_draw);
}
