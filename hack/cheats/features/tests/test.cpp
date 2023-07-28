#include "test.hpp"

#include <SDK/Enums.hpp>
#include <SDK/interfaces/interfaces.hpp>
#include <SDK/structs/Entity.hpp>
#include <SDK/IVModelInfo.hpp>
#include <SDK/CUtlVector.hpp>
#include <SDK/material.hpp>
#include <SDK/IEffects.hpp>
#include <SDK/IMaterialSystem.hpp>
#include <SDK/CTeslaInfo.hpp>
#include <SDK/IClientState.hpp>
#include <cheats/game/game.hpp>
#include <cheats/features/particle/particle.hpp>
#include <utilities/inputSystem.hpp>
#include <render/render.hpp>

#include <utilities/math/math.hpp>
#include <gamememory/memory.hpp>
#include <cheats/hooks/frameStageNotify.hpp>
#include <cheats/hooks/paintTraverse.hpp>

// place anything you want to test

namespace
{
	struct WeatherHandler : hooks::FrameStageNotify
	{
		WeatherHandler()
		{
			this->registerRun(test::run);
		}
	} /*weatherHandler*/;

	struct RenderTest : hooks::PaintTraverse
	{
		RenderTest()
		{
			this->registerRender(test::draw);
		}
	} /*renderTest*/;
}

void test::run(FrameStage stage)
{
	
	
}

void test::draw()
{
	if (!game::isAvailable())
		return;

	const Vec3& mins = memory::interfaces::gameWorld->m_WorldMins();
	const Vec3& maxs = memory::interfaces::gameWorld->m_WorldMaxs();
	const Vec3 center = (mins + maxs) * 0.5f;
	
	const float halfLength = std::abs(maxs[0] - mins[0]) * 0.5f;
	const float halfWidth = std::abs(maxs[1]  - mins[1]) * 0.5f;
	const float partHeight = std::abs(maxs[2] - mins[2]) * 0.75f;

	const Vec3 corner1 = Vec3{ center[0] + halfLength, center[1] + halfWidth, center[2] + partHeight };
	const Vec3 corner2 = Vec3{ center[0] + halfLength, center[1] - halfWidth, center[2] + partHeight };
	const Vec3 corner3 = Vec3{ center[0] - halfLength, center[1] + halfWidth, center[2] + partHeight };
	const Vec3 corner4 = Vec3{ center[0] - halfLength, center[1] - halfWidth, center[2] + partHeight };

	/*if (ImVec2 s; ImRender::worldToScreen(corner1, s))
	{
		ImRender::drawCircleFilled(s.x, s.y, 32.0f, 64, Colors::Orange);
	}
	if (ImVec2 s; ImRender::worldToScreen(corner2, s))
	{
		ImRender::drawCircleFilled(s.x, s.y, 32.0f, 64, Colors::Orange);
	}
	if (ImVec2 s; ImRender::worldToScreen(corner3, s))
	{
		ImRender::drawCircleFilled(s.x, s.y, 32.0f, 64, Colors::Orange);
	}
	if (ImVec2 s; ImRender::worldToScreen(corner4, s))
	{
		ImRender::drawCircleFilled(s.x, s.y, 32.0f, 64, Colors::Orange);
	}*/
}

/*
* adding entity: CWorld enum: 275
adding entity: CCSPlayer enum: 40
adding entity: CCSTeam enum: 43
adding entity: CCSTeam enum: 43
adding entity: CCSTeam enum: 43
adding entity: CCSTeam enum: 43
adding entity: CCSPlayerResource enum: 41
adding entity: CCSGameRulesProxy enum: 39
adding entity: CVoteController enum: 229
adding entity: CVoteController enum: 229
adding entity: CVoteController enum: 229
adding entity: CFogController enum: 78
adding entity: CSprite enum: 164
adding entity: CFuncRotating enum: 90
adding entity: CSprite enum: 164
adding entity: CSprite enum: 164
adding entity: CSprite enum: 164
adding entity: CLightGlow enum: 109
adding entity: CSpriteOriented enum: 165
adding entity: CSpriteOriented enum: 165
adding entity: CSpriteOriented enum: 165
adding entity: CSpriteOriented enum: 165
adding entity: CSpriteOriented enum: 165
adding entity: CSpriteOriented enum: 165
adding entity: CSpriteOriented enum: 165
adding entity: CSpriteOriented enum: 165
adding entity: CSpriteOriented enum: 165
adding entity: CSpriteOriented enum: 165
adding entity: CSpriteOriented enum: 165
adding entity: CSpriteOriented enum: 165
adding entity: CSpriteOriented enum: 165
adding entity: CSpriteOriented enum: 165
adding entity: CSpriteOriented enum: 165
adding entity: CSpriteOriented enum: 165
adding entity: CSpriteOriented enum: 165
adding entity: CSpriteOriented enum: 165
adding entity: CSpriteOriented enum: 165
adding entity: CSpriteOriented enum: 165
adding entity: CSpriteOriented enum: 165
adding entity: CSpriteOriented enum: 165
adding entity: CSprite enum: 164
adding entity: CSprite enum: 164
adding entity: CBaseEntity enum: 11
adding entity: CBaseEntity enum: 11
adding entity: CBaseEntity enum: 11
adding entity: CSpriteOriented enum: 165
adding entity: CDynamicProp enum: 52
adding entity: CSpriteOriented enum: 165
adding entity: CSpriteOriented enum: 165
adding entity: CDynamicProp enum: 52
adding entity: CSpriteOriented enum: 165
adding entity: CSpriteOriented enum: 165
adding entity: CDynamicProp enum: 52
adding entity: CBaseEntity enum: 11
adding entity: CBaseEntity enum: 11
adding entity: CBaseEntity enum: 11
adding entity: CBaseEntity enum: 11
adding entity: CSpriteOriented enum: 165
adding entity: CDynamicProp enum: 52
adding entity: CSpriteOriented enum: 165
adding entity: CSpriteOriented enum: 165
adding entity: CSpriteOriented enum: 165
adding entity: CBaseEntity enum: 11
adding entity: CDynamicProp enum: 52
adding entity: CDynamicProp enum: 52
adding entity: CBaseEntity enum: 11
adding entity: CSpriteOriented enum: 165
adding entity: CDynamicProp enum: 52
adding entity: CDynamicProp enum: 52
adding entity: CColorCorrection enum: 37
adding entity: CSpriteOriented enum: 165
adding entity: CSpriteOriented enum: 165
adding entity: CSpriteOriented enum: 165
adding entity: CSpriteOriented enum: 165
adding entity: CSpriteOriented enum: 165
adding entity: CSpriteOriented enum: 165
adding entity: CSpriteOriented enum: 165
adding entity: CSpriteOriented enum: 165
adding entity: CSpriteOriented enum: 165
adding entity: CDynamicProp enum: 52
adding entity: CDynamicProp enum: 52
adding entity: CDynamicProp enum: 52
adding entity: CDynamicProp enum: 52
adding entity: CDynamicProp enum: 52
adding entity: CBaseEntity enum: 11
adding entity: CBaseEntity enum: 11
adding entity: CBaseEntity enum: 11
adding entity: CBaseEntity enum: 11
adding entity: CBaseEntity enum: 11
adding entity: CDynamicProp enum: 52
adding entity: CDynamicProp enum: 52
adding entity: CWaterLODControl enum: 231
adding entity: CRopeKeyframe enum: 149
adding entity: CRopeKeyframe enum: 149
adding entity: CRopeKeyframe enum: 149
adding entity: CRopeKeyframe enum: 149
adding entity: CRopeKeyframe enum: 149
adding entity: CRopeKeyframe enum: 149
adding entity: CRopeKeyframe enum: 149
adding entity: CRopeKeyframe enum: 149
adding entity: CRopeKeyframe enum: 149
adding entity: CSpriteOriented enum: 165
adding entity: CSpriteOriented enum: 165
adding entity: CDynamicProp enum: 52
adding entity: CDynamicProp enum: 52
adding entity: CLightGlow enum: 109
adding entity: CLightGlow enum: 109
adding entity: CDynamicProp enum: 52
adding entity: CDynamicProp enum: 52
adding entity: CDynamicProp enum: 52
adding entity: CDynamicProp enum: 52
adding entity: CDynamicProp enum: 52
adding entity: CDynamicProp enum: 52
adding entity: CLightGlow enum: 109
adding entity: CLightGlow enum: 109
adding entity: CLightGlow enum: 109
adding entity: CLightGlow enum: 109
adding entity: CDynamicProp enum: 52
adding entity: CDynamicProp enum: 52
adding entity: CDynamicProp enum: 52
adding entity: CDynamicProp enum: 52
adding entity: CDynamicProp enum: 52
adding entity: CSprite enum: 164
adding entity: CSprite enum: 164
adding entity: CDynamicProp enum: 52
adding entity: CDynamicProp enum: 52
adding entity: CDynamicProp enum: 52
adding entity: CDynamicProp enum: 52
adding entity: CDynamicProp enum: 52
adding entity: CDynamicProp enum: 52
adding entity: CDynamicProp enum: 52
adding entity: CDynamicProp enum: 52
adding entity: CDynamicProp enum: 52
adding entity: CDynamicProp enum: 52
adding entity: CSpriteOriented enum: 165
adding entity: CDynamicProp enum: 52
adding entity: CPredictedViewModel enum: 140
adding entity: CDynamicProp enum: 52
adding entity: CDynamicProp enum: 52
adding entity: CSpriteOriented enum: 165
adding entity: CLightGlow enum: 109
adding entity: CLightGlow enum: 109
adding entity: CLightGlow enum: 109
adding entity: CFuncRotating enum: 90
adding entity: CSpriteOriented enum: 165
adding entity: CSpriteOriented enum: 165
adding entity: CSpriteOriented enum: 165
adding entity: CPrecipitation enum: 138
adding entity: CWeaponNegev enum: 255
adding entity: CBaseWeaponWorldModel enum: 23
adding entity: CWeaponNegev enum: 255
adding entity: CBaseWeaponWorldModel enum: 23
adding entity: CWeaponNegev enum: 255
adding entity: CBaseWeaponWorldModel enum: 23
adding entity: CWeaponNegev enum: 255
adding entity: CBaseWeaponWorldModel enum: 23
adding entity: CDynamicProp enum: 52
adding entity: CDynamicProp enum: 52
adding entity: CDynamicProp enum: 52
adding entity: CSpotlightEnd enum: 163
adding entity: CBeam enum: 24
adding entity: CSpotlightEnd enum: 163
adding entity: CBeam enum: 24
adding entity: CSpotlightEnd enum: 163
adding entity: CBeam enum: 24
adding entity: CSpotlightEnd enum: 163
adding entity: CBeam enum: 24
adding entity: CSpotlightEnd enum: 163
adding entity: CBeam enum: 24
adding entity: CSpotlightEnd enum: 163
adding entity: CBeam enum: 24
adding entity: CSpotlightEnd enum: 163
adding entity: CBeam enum: 24
adding entity: CSpotlightEnd enum: 163
adding entity: CBeam enum: 24
adding entity: CSpotlightEnd enum: 163
adding entity: CBeam enum: 24
adding entity: CSpotlightEnd enum: 163
adding entity: CBeam enum: 24
adding entity: CSpotlightEnd enum: 163
adding entity: CBeam enum: 24
adding entity: CCascadeLight enum: 35
adding entity: CBaseAnimating enum: 2
adding entity: CBreakableProp enum: 30
adding entity: CBreakableProp enum: 30
*/
/*
* adding entity: CWorld enum: 275
adding entity: CCSPlayer enum: 40
adding entity: CCSTeam enum: 43
adding entity: CCSTeam enum: 43
adding entity: CCSTeam enum: 43
adding entity: CCSTeam enum: 43
adding entity: CCSPlayerResource enum: 41
adding entity: CCSGameRulesProxy enum: 39
adding entity: CVoteController enum: 229
adding entity: CVoteController enum: 229
adding entity: CVoteController enum: 229
adding entity: CFogController enum: 78
adding entity: CSprite enum: 164
adding entity: CFuncRotating enum: 90
adding entity: CSprite enum: 164
adding entity: CSprite enum: 164
adding entity: CSprite enum: 164
adding entity: CLightGlow enum: 109
adding entity: CSpriteOriented enum: 165
adding entity: CSpriteOriented enum: 165
adding entity: CSpriteOriented enum: 165
adding entity: CSpriteOriented enum: 165
adding entity: CSpriteOriented enum: 165
adding entity: CSpriteOriented enum: 165
adding entity: CSpriteOriented enum: 165
adding entity: CSpriteOriented enum: 165
adding entity: CSpriteOriented enum: 165
adding entity: CSpriteOriented enum: 165
adding entity: CSpriteOriented enum: 165
adding entity: CSpriteOriented enum: 165
adding entity: CSpriteOriented enum: 165
adding entity: CSpriteOriented enum: 165
adding entity: CSpriteOriented enum: 165
adding entity: CSpriteOriented enum: 165
adding entity: CSpriteOriented enum: 165
adding entity: CSpriteOriented enum: 165
adding entity: CSpriteOriented enum: 165
adding entity: CSpriteOriented enum: 165
adding entity: CSpriteOriented enum: 165
adding entity: CSpriteOriented enum: 165
adding entity: CSprite enum: 164
adding entity: CSprite enum: 164
adding entity: CBaseEntity enum: 11
adding entity: CBaseEntity enum: 11
adding entity: CBaseEntity enum: 11
adding entity: CSpriteOriented enum: 165
adding entity: CDynamicProp enum: 52
adding entity: CSpriteOriented enum: 165
adding entity: CSpriteOriented enum: 165
adding entity: CDynamicProp enum: 52
adding entity: CSpriteOriented enum: 165
adding entity: CSpriteOriented enum: 165
adding entity: CDynamicProp enum: 52
adding entity: CBaseEntity enum: 11
adding entity: CBaseEntity enum: 11
adding entity: CBaseEntity enum: 11
adding entity: CBaseEntity enum: 11
adding entity: CSpriteOriented enum: 165
adding entity: CDynamicProp enum: 52
adding entity: CSpriteOriented enum: 165
adding entity: CSpriteOriented enum: 165
adding entity: CSpriteOriented enum: 165
adding entity: CBaseEntity enum: 11
adding entity: CDynamicProp enum: 52
adding entity: CDynamicProp enum: 52
adding entity: CBaseEntity enum: 11
adding entity: CSpriteOriented enum: 165
adding entity: CDynamicProp enum: 52
adding entity: CDynamicProp enum: 52
adding entity: CColorCorrection enum: 37
adding entity: CSpriteOriented enum: 165
adding entity: CSpriteOriented enum: 165
adding entity: CSpriteOriented enum: 165
adding entity: CSpriteOriented enum: 165
adding entity: CSpriteOriented enum: 165
adding entity: CSpriteOriented enum: 165
adding entity: CSpriteOriented enum: 165
adding entity: CSpriteOriented enum: 165
adding entity: CSpriteOriented enum: 165
adding entity: CDynamicProp enum: 52
adding entity: CDynamicProp enum: 52
adding entity: CDynamicProp enum: 52
adding entity: CDynamicProp enum: 52
adding entity: CDynamicProp enum: 52
adding entity: CBaseEntity enum: 11
adding entity: CBaseEntity enum: 11
adding entity: CBaseEntity enum: 11
adding entity: CBaseEntity enum: 11
adding entity: CBaseEntity enum: 11
adding entity: CDynamicLight enum: 51
adding entity: CDynamicProp enum: 52
adding entity: CDynamicProp enum: 52
adding entity: CWaterLODControl enum: 231
adding entity: CRopeKeyframe enum: 149
adding entity: CRopeKeyframe enum: 149
adding entity: CRopeKeyframe enum: 149
adding entity: CRopeKeyframe enum: 149
adding entity: CRopeKeyframe enum: 149
adding entity: CRopeKeyframe enum: 149
adding entity: CRopeKeyframe enum: 149
adding entity: CRopeKeyframe enum: 149
adding entity: CRopeKeyframe enum: 149
adding entity: CSpriteOriented enum: 165
adding entity: CSpriteOriented enum: 165
adding entity: CDynamicProp enum: 52
adding entity: CDynamicProp enum: 52
adding entity: CLightGlow enum: 109
adding entity: CLightGlow enum: 109
adding entity: CDynamicProp enum: 52
adding entity: CDynamicProp enum: 52
adding entity: CDynamicProp enum: 52
adding entity: CDynamicProp enum: 52
adding entity: CDynamicProp enum: 52
adding entity: CDynamicProp enum: 52
adding entity: CLightGlow enum: 109
adding entity: CLightGlow enum: 109
adding entity: CLightGlow enum: 109
adding entity: CLightGlow enum: 109
adding entity: CDynamicProp enum: 52
adding entity: CDynamicProp enum: 52
adding entity: CDynamicProp enum: 52
adding entity: CDynamicProp enum: 52
adding entity: CDynamicProp enum: 52
adding entity: CSprite enum: 164
adding entity: CSprite enum: 164
adding entity: CDynamicProp enum: 52
adding entity: CDynamicProp enum: 52
adding entity: CDynamicProp enum: 52
adding entity: CDynamicProp enum: 52
adding entity: CDynamicProp enum: 52
adding entity: CDynamicProp enum: 52
adding entity: CDynamicProp enum: 52
adding entity: CDynamicProp enum: 52
adding entity: CDynamicProp enum: 52
adding entity: CDynamicProp enum: 52
adding entity: CSpriteOriented enum: 165
adding entity: CDynamicProp enum: 52
adding entity: CPredictedViewModel enum: 140
adding entity: CDynamicProp enum: 52
adding entity: CDynamicProp enum: 52
adding entity: CSpriteOriented enum: 165
adding entity: CLightGlow enum: 109
adding entity: CLightGlow enum: 109
adding entity: CLightGlow enum: 109
adding entity: CFuncRotating enum: 90
adding entity: CSpriteOriented enum: 165
adding entity: CSpriteOriented enum: 165
adding entity: CSpriteOriented enum: 165
adding entity: CPrecipitation enum: 138
adding entity: CWeaponNegev enum: 255
adding entity: CBaseWeaponWorldModel enum: 23
adding entity: CWeaponNegev enum: 255
adding entity: CBaseWeaponWorldModel enum: 23
adding entity: CWeaponNegev enum: 255
adding entity: CBaseWeaponWorldModel enum: 23
adding entity: CWeaponNegev enum: 255
adding entity: CBaseWeaponWorldModel enum: 23
adding entity: CDynamicProp enum: 52
adding entity: CDynamicProp enum: 52
adding entity: CDynamicProp enum: 52
adding entity: CSpotlightEnd enum: 163
adding entity: CBeam enum: 24
adding entity: CSpotlightEnd enum: 163
adding entity: CBeam enum: 24
adding entity: CSpotlightEnd enum: 163
adding entity: CBeam enum: 24
adding entity: CSpotlightEnd enum: 163
adding entity: CBeam enum: 24
adding entity: CSpotlightEnd enum: 163
adding entity: CBeam enum: 24
adding entity: CSpotlightEnd enum: 163
adding entity: CBeam enum: 24
adding entity: CSpotlightEnd enum: 163
adding entity: CBeam enum: 24
adding entity: CSpotlightEnd enum: 163
adding entity: CBeam enum: 24
adding entity: CSpotlightEnd enum: 163
adding entity: CBeam enum: 24
adding entity: CSpotlightEnd enum: 163
adding entity: CBeam enum: 24
adding entity: CSpotlightEnd enum: 163
adding entity: CBeam enum: 24
adding entity: CCascadeLight enum: 35
adding entity: CBaseAnimating enum: 2
adding entity: CBreakableProp enum: 30
adding entity: CBreakableProp enum: 30
*/