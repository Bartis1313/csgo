#pragma once

#include "enums.hpp"
#include "cfgWeapon.hpp"
#include "cfgcolor.hpp"
#include "key.hpp"
#include "cfgBeam.hpp"

#include <utilities/tools/wrappers.hpp>
#include <utilities/utilities.hpp>
#include <magic_enum.hpp>

#include <memory>
#include <cstdint>
#include <array>

#define USE_NAMESPACE_VARS(cl, name) \
namespace vars { \
	inline std::unique_ptr<cl> name{ std::make_unique<cl>() }; \
}

#define ADD_MEMBER_PTR(_structName, memberName) std::unique_ptr<_structName> memberName{ std::make_unique<_structName>() };

struct VarAim
{
	std::array<CfgWeapon, magic_enum::enum_count<WeaponList>()> weapons;
	bool useKey{ false };
};
USE_NAMESPACE_VARS(VarAim, aim);

struct VarAimPaint
{
	bool enabledFov{ false };
	bool enabledPoint{ false };
	CfgColor colorFov{ Colors::LightBlue };
	CfgColor colorPoint{ Colors::LightBlue };
};
USE_NAMESPACE_VARS(VarAimPaint, aimPaint);

struct VarVisuals
{
	struct VarChams
	{
		bool enabled{ false };
		bool players{ false };
		int indexPlayers{ 0 };
		CfgColor colorPlayers{ Color{ 255, 0, 255, 255 } };
		bool enabledXQZPlayers{ false };
		CfgColor colorXQZPlayers{ { 0, 100, 255, 255 } };
		bool enabledWeapons{ false };
		bool weaponHide{ false };
		int indexWeapons{ 0 };
		CfgColor colorWeapons{ Color{ 255, 0, 255, 255 } };
		bool armsHide{ false };
		bool enabledArms{ false };
		CfgColor colorArms{ Color{ 255, 0, 255, 255 } };
		int indexArms{ 0 };
		int indexBacktrack{ 0 };
		bool enabledBacktrack{ false };
		CfgColor colorBacktrack{ Color{ 255, 0, 255, 255 } };
		int modeBacktrack{ 0 };
		bool rainbowBacktrack{ false };
		float rainbowBacktrackSpeed{ 0.5f };
	};
	ADD_MEMBER_PTR(VarChams, chams);

	struct VarGlow
	{
		bool enabled{ false };
		bool fullBloom{ false };
		CfgColor colorPlayer{ Colors::Purple };
		std::array<float, magic_enum::enum_count<GlowRenderPulse>()> pulseSpeeeds{ utilities::getFilledArray<float, magic_enum::enum_count<GlowRenderPulse>()>(12.0f) };
		bool occluded{ false };
		bool unoccluded{ false };
		float exponent{ 1.0f };
		float saturation{ 1.0f };
		float C0_X{ 1.0f };
		float thickness{ 1.0f };
		std::array<bool, magic_enum::enum_count<GlowRenderStyle>()> usedMats{ utilities::getFilledArray<bool, magic_enum::enum_count<GlowRenderStyle>()>(false) };
		std::array<bool, magic_enum::enum_count<GlowRenderStyle>()> ignorez{ utilities::getFilledArray<bool, magic_enum::enum_count<GlowRenderStyle>()>(false) };
	};
	ADD_MEMBER_PTR(VarGlow, glow);

	struct VarEsp
	{
		struct VarBoxes
		{
			bool enabled{ false };
			int mode{ 0 };
			CfgColor color{ Colors::Purple };
			CfgColor fill{ Color{ 0, 0, 0, 140 } };
			bool multiColor{ false };
			bool outline{ false };
			float multiColorSpeed{ 1.0f };
		};
		ADD_MEMBER_PTR(VarBoxes, boxes);

		struct VarHealthBar
		{
			bool enabled{ false };
		};
		ADD_MEMBER_PTR(VarHealthBar, healthBar);

		struct VarArmorBar
		{
			bool enabled{ false };
		};
		ADD_MEMBER_PTR(VarArmorBar, armorBar);

		struct VarWeaponBar
		{
			bool enabled{ false };
			bool translate{ false };
			CfgColor text{ Colors::White };
			CfgColor bar{ Colors::Turquoise };
		};
		ADD_MEMBER_PTR(VarWeaponBar, weaponBar);

		struct VarName
		{
			bool enabled{ false };
		};
		ADD_MEMBER_PTR(VarName, nameBar);

		struct VarSkeleton
		{
			bool enabled{ false };
			CfgColor color{ Colors::White };
			bool showDebug{ false };
		};
		ADD_MEMBER_PTR(VarSkeleton, skeleton);

		struct VarDlight
		{
			bool enabled{ false };
			CfgColor color{ Color{ 20, 70, 150, 255 } };
			float radius{ 50.0f };
			float decay{ 30.0f };
			float exponent{ 8.0f };
		};
		ADD_MEMBER_PTR(VarDlight, dlight);

		struct VarExtraInfo
		{
			bool enabled{ false };
		};
		ADD_MEMBER_PTR(VarExtraInfo, extraInfo);

		struct VarFlags
		{
			std::array<bool, magic_enum::enum_count<EspFlags>()> flags{ false };
			bool enabled{ false };
		};
		ADD_MEMBER_PTR(VarFlags, flags);

		struct VarLasers
		{
			bool enabled{ false };
		};
		ADD_MEMBER_PTR(VarLasers, lasers);

		struct VarChecks
		{
			bool dead{ false };
			bool smoke{ false };
			bool visible{ false };
			float flashLimit{ 120.0f };
		};
		ADD_MEMBER_PTR(VarChecks, checks);

		struct VarDropped
		{
			bool enabled{ false };
			CfgColor color{ Colors::White };
			std::array<bool, magic_enum::enum_count<DroppedFlags>()> flags{ false };
		};
		ADD_MEMBER_PTR(VarDropped, dropped);
	};
	ADD_MEMBER_PTR(VarEsp, esp);

	struct VarSound
	{
		bool enabled{ false };
		CfgColor color{ Colors::LightBlue };
		float time{ 10.0f };
		float maxDist{ 50.0f };
		float maxDistLine{ 80.0f };
		CfgColor colorLine{ Colors::White };
	};
	ADD_MEMBER_PTR(VarSound, sound);

	struct VarDormacy
	{
		float time{ 1.0f };
		float limit{ 15.0f };
	};
	ADD_MEMBER_PTR(VarDormacy, dormacy);

	struct VarWorld
	{
		struct VarBomb
		{
			bool enabled{ false };
			CfgColor background{ Colors::Black };
		};
		ADD_MEMBER_PTR(VarBomb, bomb);

		struct VarProjectiles
		{
			bool enabled{ false };
			CfgColor flash{ Color{ 130, 0, 200, 255 } };
			CfgColor nade{ Color{ 130, 180, 20, 255 } };
			CfgColor molotov{ Color{ 130, 200, 120, 255 } };
			CfgColor smoke{ Color{ 20, 70, 30, 255 } };
			CfgColor decoy{ Color{ 0, 30, 60, 255 } };
		};
		ADD_MEMBER_PTR(VarProjectiles, projectiles);

		struct VarSky
		{
			int indexNormal{ 0 };
			int indexCustom{ 0 };
			bool removeSky{ false };
		};
		ADD_MEMBER_PTR(VarSky, sky);

		struct VarModulate
		{
			bool enabled{ false };
			CfgColor texture{ Colors::White };
			CfgColor prop{ Colors::White };
			CfgColor sky{ Colors::White };
			float shader{ 100.0f };
		};
		ADD_MEMBER_PTR(VarModulate, modulate);

		struct VarMolotov
		{
			bool enabled{ false };
			bool triangulation{ false };
			CfgColor color{ Colors::Red.getColorEditAlpha(0.3f) };
			CfgColor colorText{ Colors::White };
		};
		ADD_MEMBER_PTR(VarMolotov, molotov);

		struct VarSmoke
		{
			bool enabled{ false };
			CfgColor color{ Colors::White };
		};
		ADD_MEMBER_PTR(VarSmoke, smoke);

		struct VarZeus
		{
			bool enabled{ false };
			bool party{ false };
			bool tracing{ false };
			CfgColor color{ Colors::Palevioletred, true, 4.0f };
		};
		ADD_MEMBER_PTR(VarZeus, zeus);

		struct VarParticles
		{
			bool enabledBlood{ false };
			CfgColor colorBlood{ Colors::Purple };
			bool enabledMolotov{ false };
			CfgColor colorMolotov{ Colors::Purple };
			bool enabledSmoke{ false };
			CfgColor colorSmoke{ Colors::Purple };
		};
		ADD_MEMBER_PTR(VarParticles, particles);

		struct VarTracer
		{
			bool enabled{ false };
			CfgBeam beamTracer
			{
				0U,
				"4|8",
				2.0f,
				Colors::LightBlue,
				2.0f,
				1.0f,
				2.0f,
				1.0f,
				0.0f,
				60.0f,
				2
			};
		};
		ADD_MEMBER_PTR(VarTracer, tracer);

		struct VarImpacts
		{
			bool enabledClient{ false };
			float timeClient{ 3.0f };
			CfgColor colorClient{ Colors::Cyan };
			CfgColor colorClientFill{ Colors::Red };
			bool enabledLocal{ false };
			float timeLocal{ 3.0f };
			CfgColor colorLocal{ Colors::Cyan };
			CfgColor colorLocalFill{ Colors::Red };
		};
		ADD_MEMBER_PTR(VarImpacts, impacts);

		struct VarFog
		{
			bool enabled{ false };
			float distance{ 500.0f };
			CfgColor color{ Colors::Purple };
		};
		ADD_MEMBER_PTR(VarFog, fog);

		struct VarScreenEffect
		{
			int index{ 0 };
			float param{ 1.0f };
			CfgColor color{ Colors::LightBlue };
		};
		ADD_MEMBER_PTR(VarScreenEffect, screenEffect);

		struct VarTone
		{
			bool enabled{ false };
			float min{ 0.5f };
			float max{ 0.5f };
			float bloom{ 0.0f };
		};
		ADD_MEMBER_PTR(VarTone, tone);

		struct VarWeather
		{
			bool enabled{ false };
			float length{ 0.0f };
			float rainSpeed{ 0.0f };
			float radius{ 0.0f };
			float width{ 0.0f };
			float velocity{ 0.0f };
			float alpha{ 0.0f };
			float windSpeed{ 0.0f };
		};
		ADD_MEMBER_PTR(VarWeather, weather);

		struct VarAmbient
		{
			bool enabled{ false };
			CfgColor color{ Colors::LightBlue };
		};
		ADD_MEMBER_PTR(VarAmbient, ambient);
	};
	ADD_MEMBER_PTR(VarWorld, world);
};
USE_NAMESPACE_VARS(VarVisuals, visuals);

struct VarKeys
{
	bool enabledX88Menu{ true };
	Key aimbot{ KeyMode::DOWN, VK_LBUTTON };
	Key menu{ KeyMode::TOGGLE, VK_INSERT };
	Key panic{ KeyMode::DOWN, VK_DELETE };
	Key console{ KeyMode::TOGGLE, VK_HOME };
	Key freeLook{ KeyMode::DOWN, VK_MENU };
	Key mirrorCam{ KeyMode::TOGGLE, VK_F1 };
	Key freeCam{ KeyMode::TOGGLE, VK_F2 };
	Key flashLight{ KeyMode::TOGGLE, 0x4C }; // L key
	Key thirdP{ KeyMode::TOGGLE, 0x56 }; // V key
};
USE_NAMESPACE_VARS(VarKeys, keys);

struct VarBacktrack
{
	bool enabled{ false };
	float time{ 200.0f };
	bool smoke{ true };
	float flashLimit{ 120.0f };
};
USE_NAMESPACE_VARS(VarBacktrack, backtrack);

struct VarMisc
{
	struct VarLogs
	{
		float time{ 4.0f };
		bool enabled{ false };
	};
	ADD_MEMBER_PTR(VarLogs, logs);

	struct VarFov
	{
		float value{ 0.0f };
	};
	ADD_MEMBER_PTR(VarFov, fov);

	struct VarThridP
	{
		bool enabled{ false };
		float distance{ 200.0f };
		float x{ 0.0f };
		float y{ 0.0f };
	};
	ADD_MEMBER_PTR(VarThridP, thirdp);

	struct VarCrosshair
	{
		int index{ 0 };
	};
	ADD_MEMBER_PTR(VarCrosshair, crosshair);

	struct VarFakeLatency
	{
		bool enabled{ false };
		float amount{ 200.0f };
	};
	ADD_MEMBER_PTR(VarFakeLatency, fakeLatency);

	struct VarBunnyHop
	{
		bool enabled{ false };
		int chance{ 0 };
		int indexStrafe{ 0 };
	};
	ADD_MEMBER_PTR(VarBunnyHop, bunnyHop);

	struct VarHitmarker
	{
		bool enabled{ false };
		bool play{ false };
		bool enabled3D{ false };
		bool enabledResize{ false };
		CfgColor colorNormal{ Colors::White };
		CfgColor colorHead{ Colors::Pink };
		CfgColor colorDead{ Colors::Green };
		float time{ 0.8f };
	};
	ADD_MEMBER_PTR(VarHitmarker, hitmarker);

	struct VarScope
	{
		bool enabled{ false };
	};
	ADD_MEMBER_PTR(VarScope, scope);

	struct VarPlots
	{
		bool enabledFps{ false };
		bool enabledVelocity{ false };
		bool transparencyVelocity{ false };
		CfgColor colorVelocity{ Colors::White };
		CfgColor colorFPS{ Colors::White };
		int sizeFps{ 300 };
		int sizeVelocity{ 300 };
	};
	ADD_MEMBER_PTR(VarPlots, plots);

	struct VarInfo
	{
		bool enabled{ false };
	};
	ADD_MEMBER_PTR(VarInfo, info);

	struct VarPlayerList
	{
		bool enabled{ false };
		bool health{ false };
		bool teamID{ false };
		bool money{ false };
		bool lastPlace{ false };
	};
	ADD_MEMBER_PTR(VarPlayerList, playerList);

	struct VarSpectactorList
	{
		bool enabled{ false };
	};
	ADD_MEMBER_PTR(VarSpectactorList, spectactorList);

	struct VarAimWarn
	{
		bool enabled{ false };
	};
	ADD_MEMBER_PTR(VarAimWarn, aimWarn);

	struct VarRadar
	{
		bool enabled{ false };
		CfgColor colorLine{ Colors::White };
		CfgColor colorPlayer{ Colors::Purple };
		float thickness{ 5.0f };
		float length{ 20.0f };
		float scale{ 1.8f };
		bool ranges{ true };
		float size{ 200.0f };
	};
	ADD_MEMBER_PTR(VarRadar, radar);

	struct VarTrail
	{
		int mode{ 0 };
		bool enabled{ false };
		CfgColor color{ Colors::Coral };
		bool rainbow{ false };
		float rainbowSpeed{ 3.0f };
		float beamSpeed{ 2.0f };
		float time{ 4.0f };
	};
	ADD_MEMBER_PTR(VarTrail, trail);

	struct VarNades
	{
		bool enabledPred{ false };
		bool predAlways{ false };
		CfgColor colorPredLine{ Colors::LightBlue };
		CfgColor colorPredBoxFill{ Colors::Green };
		CfgColor colorPredBox{ Colors::Green };
		bool enabledTracer{ false };
		CfgColor colorTracer{ Colors::LightBlue };
		float tracerDist{ 30.0f };
		bool tracerWarn{ false };
	};
	ADD_MEMBER_PTR(VarNades, nade);

	struct VarHat
	{
		bool enabled{ false };
		bool rainbow{ false };
		float rainbowSpeed{ 3.0f };
		float size{ -30.0f };
		float radius{ 20.0f };
		int rainbowAlpha{ 70 };
		int rainbowLinesAlpha{ 180 };
		CfgColor colorTriangle{ Colors::Palevioletred };
		CfgColor colorLine{ Colors::Palevioletred };
	};
	ADD_MEMBER_PTR(VarHat, hat);

	struct VarDiscord
	{
		bool enabled{ false };
	};
	ADD_MEMBER_PTR(VarDiscord, discord);

	struct VarMotionBlur
	{
		bool enabled{ false };
		bool forward{ false };
		float fallingIntensity{ 1.0f };
		float fallingMin{ 10.0f };
		float fallingMax{ 20.0f };
		float strength{ 1.0f };
		float rotationIntensity{ 0.15f };
		float rollIntensity{ 0.3f };
	};
	ADD_MEMBER_PTR(VarMotionBlur, motionBlur);

	struct VarFreeLook
	{
		bool enabled{ false };
	};
	ADD_MEMBER_PTR(VarFreeLook, freeLook);

	struct VarMirrorCam
	{
		bool enabled{ false };
		bool onKey{ false };
	};
	ADD_MEMBER_PTR(VarMirrorCam, mirrorCam);

	struct VarFreeCam
	{
		bool enabled{ false };
		float speed{ 1.0f };
	};
	ADD_MEMBER_PTR(VarFreeCam, freeCam);

	struct VarFlashlight
	{
		bool enabled{ false };
		bool bigMode{ false };
		float fov{ 30.0f };
	};
	ADD_MEMBER_PTR(VarFlashlight, flashLight);

	struct DisableItems
	{
		bool interpolate{ false };
		float flashPercentage{ 1.0f };
	};
	ADD_MEMBER_PTR(DisableItems, disableItems);
};
USE_NAMESPACE_VARS(VarMisc, misc);

struct ImGuiStyle;

#include <imgui.h>

struct VarStyling
{
	int index{ 0 };
	bool background{ false };
	float speed{ 2.0f };
	float distance{ 200.0f };
	int size{ 150 };
	CfgColor color1{ Color{ 220, 10, 100, 255 } };
	CfgColor color2{ Color{ 65, 75, 105, 255 } };
	CfgColor color3{ Color{ 220, 240, 240, 255 } };
	ImGuiStyle imStyle;
	std::array<CfgColor, 4> groupPanelBackground{ utilities::getFilledArray<CfgColor, 4>(Color{ 0, 140, 250, 0 }) };
	CfgColor groupPanelLines{ Colors::White };
	float smoothAlpha{ 0.1f };
	float smoothOutline{ 0.1f };
	float smoothPop{ 0.0f };
	float speedMove{ 10.0f };
};
USE_NAMESPACE_VARS(VarStyling, styling);

#undef USE_NAMESPACE_VARS
#undef ADD_MEMBER_PTR