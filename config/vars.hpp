#pragma once

#include "enums.hpp"
#include "cfgWeapon.hpp"
#include "cfgcolor.hpp"
#include "key.hpp"
#include "cfgBeam.hpp"

#include <utilities/tools/wrappers.hpp>
#include <dependencies/magic_enum.hpp>

#include <memory>
#include <cstdint>
#include <array>

#define USE_NAMESPACE_VARS(cl, name) \
namespace vars { \
	inline std::unique_ptr<cl> name{ new cl }; \
}

struct VarAim
{
	std::array<CfgWeapon, magic_enum::enum_count<WeaponList>()> weapons;
	bool useKey = false;
};
USE_NAMESPACE_VARS(VarAim, aim);

struct VarAimPaint
{
	bool enabledFov = false;
	bool enabledPoint = false;
	CfgColor colorFov = Colors::LightBlue;
	CfgColor colorPoint = Colors::LightBlue;
};
USE_NAMESPACE_VARS(VarAimPaint, aimPaint);

struct VarVisuals
{
	struct VarChams
	{
		bool enabled = false;
		bool players = false;
		int indexPlayers = 0;
		CfgColor colorPlayers = Color{ 255, 0, 255, 255 };
		bool enabledXQZPlayers = false;
		CfgColor colorXQZPlayers = Color{ 0, 100, 255, 255 };
		bool enabledWeapons = false;
		bool weaponHide = false;
		int indexWeapons = 0;
		CfgColor colorWeapons = Color{ 255, 0, 255, 255 };
		bool armsHide = false;
		bool enabledArms = false;
		CfgColor colorArms = Color{ 255, 0, 255, 255 };
		int indexArms = 0;
		int indexBacktrack = 0;
		bool enabledBacktrack = false;
		CfgColor colorBacktrack = Color{ 255, 0, 255, 255 };
		int modeBacktrack = 0;
		bool rainbowBacktrack = false;
		float rainbowBacktrackSpeed = 0.5f;
	};
	std::unique_ptr<VarChams> chams{ new VarChams };

	struct VarGlow
	{
		bool enabled = false;
		CfgColor colorPlayer = Colors::Purple;
	};
	std::unique_ptr<VarGlow> glow{ new VarGlow };

	struct VarEsp
	{
		struct VarBoxes
		{
			bool enabled = false;
			int mode = 0;
			CfgColor color = Colors::Purple;
			CfgColor fill = Color{ 0, 0, 0, 140 };
			bool multiColor = false;
			bool outline = false;
			float multiColorSpeed = 1.0f;
		};
		std::unique_ptr<VarBoxes> boxes{ new VarBoxes };

		struct VarHealthBar
		{
			bool enabled = false;
		};
		std::unique_ptr<VarHealthBar> healthBar{ new VarHealthBar };

		struct VarArmorBar
		{
			bool enabled = false;
		};
		std::unique_ptr<VarArmorBar> armorBar{ new VarArmorBar };

		struct VarWeaponBar
		{
			bool enabled = false;
			bool translate = false;
			CfgColor text = Colors::White;
			CfgColor bar = Colors::Turquoise;
		};
		std::unique_ptr<VarWeaponBar> weaponBar{ new VarWeaponBar };

		struct VarName
		{
			bool enabled = false;
		};
		std::unique_ptr<VarName> nameBar{ new VarName };

		struct VarSkeleton
		{
			bool enabled = false;
			CfgColor color = Colors::White;
			bool showDebug = false;
		};
		std::unique_ptr<VarSkeleton> skeleton{ new VarSkeleton };

		struct VarDlight
		{
			bool enabled = false;
			CfgColor color = Color{ 20, 70, 150, 255 };
			float radius = 50.0f;
			float decay = 30.0f;
			float exponent = 8.0f;
		};
		std::unique_ptr<VarDlight> dlight{ new VarDlight };

		struct VarExtraInfo
		{
			bool enabled = false;
		};
		std::unique_ptr<VarExtraInfo> extraInfo{ new VarExtraInfo };

		struct VarFlags
		{
			std::array<bool, magic_enum::enum_count<EspFlags>()> flags{ false };
			bool enabled = false;
		};
		std::unique_ptr<VarFlags> flags{ new VarFlags };

		struct VarLasers
		{
			bool enabled = false;
		};
		std::unique_ptr<VarLasers> lasers{ new VarLasers };

		struct VarChecks
		{
			bool dead = false;
			bool smoke = false;
			bool visible = false;
			float flashLimit = 120.0f;
		};
		std::unique_ptr<VarChecks> checks{ new VarChecks };

		struct VarDropped
		{
			bool enabled = false;
			CfgColor color = Colors::White;
			std::array<bool, magic_enum::enum_count<DroppedFlags>()> flags{ false };
		};
		std::unique_ptr<VarDropped> dropped{ new VarDropped };
	};
	std::unique_ptr<VarEsp> esp{ new VarEsp };

	struct VarSound
	{
		bool enabled = false;
		CfgColor color = Colors::LightBlue;
		float time = 10.0f;
		float maxDist = 50.0f;
		float maxDistLine = 80.0f;
		CfgColor colorLine = Colors::White;
	};
	std::unique_ptr<VarSound> sound{ new VarSound };

	struct VarDormacy
	{
		float time = 1.0f;
		float limit = 15.0f;
	};
	std::unique_ptr<VarDormacy> dormacy{ new VarDormacy };

	struct VarWorld
	{
		struct VarBomb
		{
			bool enabled = false;
			CfgColor background = Colors::Black;
		};
		std::unique_ptr<VarBomb> bomb{ new VarBomb };

		struct VarProjectiles
		{
			bool enabled = false;
			CfgColor flash = Color{ 130, 0, 200, 255 };
			CfgColor nade = Color{ 130, 180, 20, 255 };
			CfgColor molotov = Color{ 130, 200, 120, 255 };
			CfgColor smoke = Color{ 20, 70, 30, 255 };
			CfgColor decoy = Color{ 0, 30, 60, 255 };
		};
		std::unique_ptr<VarProjectiles> projectiles{ new VarProjectiles };

		struct VarSky
		{
			int indexNormal = 0U;
			int indexCustom = 0U;
			bool removeSky = false;
		};
		std::unique_ptr<VarSky> sky{ new VarSky };

		struct VarModulate
		{
			bool enabled = false;
			CfgColor texture = Colors::White;
			CfgColor prop = Colors::White;
			CfgColor sky = Colors::White;
			float shader = 100.0f;
		};
		std::unique_ptr<VarModulate> modulate{ new VarModulate };

		struct VarMolotov
		{
			bool enabled = false;
			CfgColor color = Colors::Red.getColorEditAlpha(0.3f);
			CfgColor colorText = Colors::White;
		};
		std::unique_ptr<VarMolotov> molotov{ new VarMolotov };

		struct VarSmoke
		{
			bool enabled = false;
			CfgColor color = Colors::White;
		};
		std::unique_ptr<VarSmoke> smoke{ new VarSmoke };

		struct VarZeus
		{
			bool enabled = false;
			bool party = false;
			bool tracing = false;
			CfgColor color = CfgColor{ Colors::Palevioletred, true, 4.0f };
		};
		std::unique_ptr<VarZeus> zeus{ new VarZeus };

		struct VarParticles
		{
			bool enabledBlood = false;
			CfgColor colorBlood = Colors::Purple;
			bool enabledMolotov = false;
			CfgColor colorMolotov = Colors::Purple;
			bool enabledSmoke = false;
			CfgColor colorSmoke = Colors::Purple;
		};
		std::unique_ptr<VarParticles> particles{ new VarParticles };

		struct VarTracer
		{
			bool enabled = false;
			CfgBeam beamTracer = CfgBeam
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
		std::unique_ptr<VarTracer> tracer{ new VarTracer };

		struct VarImpacts
		{
			bool enabledClient = false;
			float timeClient = 3.0f;
			CfgColor colorClient = Colors::Cyan;
			CfgColor colorClientFill = Colors::Red;
			bool enabledLocal = false;
			float timeLocal = 3.0f;
			CfgColor colorLocal = Colors::Cyan;
			CfgColor colorLocalFill = Colors::Red;
		};
		std::unique_ptr<VarImpacts> impacts{ new VarImpacts };

		struct VarFog
		{
			bool enabled = false;
			float distance = 500.0f;
			CfgColor color = Colors::Purple;
		};
		std::unique_ptr<VarFog> fog{ new VarFog };

		struct VarScreenEffect
		{
			int index = 0U;
			float param = 1.0f;
			CfgColor color = Colors::LightBlue;
		};
		std::unique_ptr<VarScreenEffect> screenEffect{ new VarScreenEffect };

		struct VarTone
		{
			bool enabled = false;
			float min = 0.5f;
			float max = 0.5f;
			float bloom = 0.0f;
		};
		std::unique_ptr<VarTone> tone{ new VarTone };

		struct VarWeather
		{
			bool enabled = false;
			float length = 0.0f;
			float rainSpeed = 0.0f;
			float radius = 0.0f;
			float width = 0.0f;
			float velocity = 0.0f;
			float alpha = 0.0f;
			float windSpeed = 0.0f;
		};
		std::unique_ptr<VarWeather> weather{ new VarWeather };

		struct VarAmbient
		{
			bool enabled = false;
			CfgColor color = Colors::LightBlue;
		};
		std::unique_ptr<VarAmbient> ambient{ new VarAmbient };
	};
	std::unique_ptr<VarWorld> world{ new VarWorld };
};
USE_NAMESPACE_VARS(VarVisuals, visuals);

struct VarKeys
{
	bool enabledX88Menu = true;
	Key aimbot = Key{ KeyMode::DOWN, VK_LBUTTON };
	Key menu = Key{ KeyMode::TOGGLE, VK_INSERT };
	Key panic = Key{ KeyMode::DOWN, VK_DELETE };
	Key console = Key{ KeyMode::TOGGLE, VK_HOME };
	Key freeLook = Key{ KeyMode::DOWN, VK_MENU };
	Key mirrorCam = Key{ KeyMode::TOGGLE, VK_F1 };
	Key freeCam = Key{ KeyMode::TOGGLE, VK_F2 };
	Key flashLight = Key{ KeyMode::TOGGLE, 0x4C }; // L key
	Key thirdP = Key{ KeyMode::TOGGLE, 0x56 }; // V key
};
USE_NAMESPACE_VARS(VarKeys, keys);

struct VarBacktrack
{
	bool enabled = false;
	float time = 200.0f;
	bool smoke = true;
	float flashLimit = 120.0f;
};
USE_NAMESPACE_VARS(VarBacktrack, backtrack);

struct VarMisc
{
	struct VarLogs
	{
		float time = 4.0f;
		bool enabled = false;
	};
	std::unique_ptr<VarLogs> logs{ new VarLogs };

	struct VarFov
	{
		float value = 0.0f;
	};
	std::unique_ptr<VarFov> fov{ new VarFov };

	struct VarThridP
	{
		bool enabled = false;
		float distance = 200.0f;
		float x = 0.0f;
		float y = 0.0f;
	};
	std::unique_ptr<VarThridP> thirdp{ new VarThridP };

	struct VarCrosshair
	{
		int index = 0;
	};
	std::unique_ptr<VarCrosshair> crosshair{ new VarCrosshair };

	struct VarFakeLatency
	{
		bool enabled = false;
		float amount = 200.0f;
	};
	std::unique_ptr<VarFakeLatency> fakeLatency{ new VarFakeLatency };

	struct VarBunnyHop
	{
		bool enabled = false;
		int chance = 0;
		int indexStrafe = 0;
	};
	std::unique_ptr<VarBunnyHop> bunnyHop{ new VarBunnyHop };

	struct VarHitmarker
	{
		bool enabled = false;
		bool play = false;
		bool enabled3D = false;
		bool enabledResize = false;
		CfgColor colorNormal = Colors::White;
		CfgColor colorHead = Colors::Pink;
		CfgColor colorDead = Colors::Green;
		float time = 0.8f;
	};
	std::unique_ptr<VarHitmarker> hitmarker{ new VarHitmarker };

	struct VarScope
	{
		bool enabled = false;
	};
	std::unique_ptr<VarScope> scope{ new VarScope };

	struct VarPlots
	{
		bool enabledFps = false;
		bool enabledVelocity = false;
		CfgColor colorVelocity = Colors::White;
		CfgColor colorFPS = Colors::White;
		float fpsCap = false;
		bool fpsCustom = false;
		bool velocityCustom = false;
		bool velocityCap = false;
	};
	std::unique_ptr<VarPlots> plots{ new VarPlots };

	struct VarInfo
	{
		bool enabled = false;
	};
	std::unique_ptr<VarInfo> info{ new VarInfo };

	struct VarAimWarn
	{
		bool enabled = false;
	};
	std::unique_ptr<VarAimWarn> aimWarn{ new VarAimWarn };

	struct VarRadar
	{
		bool enabled = false;
		CfgColor colorLine = Colors::White;
		CfgColor colorPlayer = Colors::Purple;
		float thickness = 5.0f;
		float length = 20.0f;
		float scale = 1.8f;
		bool ranges = true;
		float size = 200.0f;
	};
	std::unique_ptr<VarRadar> radar{ new VarRadar };

	struct VarTrail
	{
		int mode = 0;
		bool enabled = false;
		CfgColor color = Colors::Coral;
		bool rainbow = false;
		float rainbowSpeed = 3.0f;
		float beamSpeed = 2.0f;
		float time = 4.0f;
	};
	std::unique_ptr<VarTrail> trail{ new VarTrail };

	struct VarNades
	{
		bool enabledPred = false;
		bool predAlways = false;
		CfgColor colorPredLine = Colors::LightBlue;
		CfgColor colorPredBoxFill = Colors::Green;
		CfgColor colorPredBox = Colors::Green;
		bool enabledTracer = false;
		CfgColor colorTracer = Colors::LightBlue;
		float tracerDist = 30.0f;
		bool tracerWarn = false;
	};
	std::unique_ptr<VarNades> nade{ new VarNades };

	struct VarHat
	{
		bool enabled = false;
		bool rainbow = false;
		float rainbowSpeed = 3.0f;
		float size = -30.0f;
		float radius = 20.0f;
		int rainbowAlpha = 70;
		int rainbowLinesAlpha = 180;
		CfgColor colorTriangle = Colors::Palevioletred;
		CfgColor colorLine = Colors::Palevioletred;
	};
	std::unique_ptr<VarHat> hat{ new VarHat };

	struct VarDiscord
	{
		bool enabled = false;
	};
	std::unique_ptr<VarDiscord> discord{ new VarDiscord };

	struct VarMotionBlur
	{
		bool enabled = false;
		bool forward = false;
		float fallingIntensity = 1.0f;
		float fallingMin = 10.0f;
		float fallingMax = 20.0f;
		float strength = 1.0f;
		float rotationIntensity = 0.15f;
		float rollIntensity = 0.3f;
	};
	std::unique_ptr<VarMotionBlur> motionBlur{ new VarMotionBlur };

	struct VarFreeLook
	{
		bool enabled = false;
	};
	std::unique_ptr<VarFreeLook> freeLook{ new VarFreeLook };

	struct VarMirrorCam
	{
		bool enabled = false;
		bool onKey = false;
	};
	std::unique_ptr<VarMirrorCam> mirrorCam{ new VarMirrorCam };

	struct VarFreeCam
	{
		bool enabled = false;
		float speed = 1.0f;
	};
	std::unique_ptr<VarFreeCam> freeCam{ new VarFreeCam };

	struct VarFlashlight
	{
		bool enabled = false;
		bool bigMode = false;
		float fov = 30.0f;
	};
	std::unique_ptr<VarFlashlight> flashLight{ new VarFlashlight };
};
USE_NAMESPACE_VARS(VarMisc, misc);

struct VarStyling
{
	int index = 0;
	bool background = false;
	float speed = 2.0f;
	float distance = 200.0f;
	int size = 150;
	CfgColor color1 = Color{ 220, 10, 100, 255 };
	CfgColor color2 = Color{ 65, 75, 105, 255 };
	CfgColor color3 = Color{ 220, 240, 240, 255 };
};
USE_NAMESPACE_VARS(VarStyling, styling);

#undef USE_NAMESPACE_VARS