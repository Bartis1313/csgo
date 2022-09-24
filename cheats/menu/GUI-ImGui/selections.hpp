#pragma once

#include <array>

namespace selections
{
	inline std::array aimbotHitboxes =
	{
		"NEAREST",
		"HEAD",
		"CHEST"
	};

	inline std::array espNames =
	{
		"OFF",
		"2D BOX",
		"2D FILLED",
		"3D BOX",
		"3D FILLED"
	};

	inline std::array crossHairNames =
	{
		"OFF",
		"Static",
		"Recoil",
		"Engine",
		"Spread"
	};

	inline std::array chamsNames =
	{
		"Flat",
		"Generic",
		"Glow",
		"Metalic",
		"Pearlescent"
	};

	inline std::array btChams =
	{
		"Stable",
		"Last tick",
		"Rainbow",
		"Color change"
	};

	inline std::array styleNames =
	{
		"Cherry",
		"Dark",
		"Classic",
		"White"
	};

	// https://developer.valvesoftware.com/wiki/Counter-Strike:_Global_Offensive_Sky_List - i did not check if they are up to date
	// to use custom skybox, you need to first load it in root folder and then to actually force the sky you should bypass file checks.
	inline std::array skyboxes =
	{
		"none",
		"cs_baggage_skybox_",
		"cs_tibet",
		"vietnam",
		"sky_lunacy",
		"embassy",
		"italy",
		"jungle",
		"office",
		"sky_cs15_daylight01_hdr",
		"sky_cs15_daylight02_hdr",
		"sky_day02_05",
		"nukeblank",
		"dustblank",
		"sky_venice",
		"sky_cs15_daylight03_hdr",
		"sky_cs15_daylight04_hdr",
		"sky_csgo_cloudy01",
		"sky_csgo_night02",
		"sky_csgo_night02b",
		"vertigo",
		"vertigoblue_hdr",
		"sky_dust",
		"sky_hr_aztec"
	};

	inline std::array trails =
	{
		"Beam",
		"Line",
		"Splash"
	};

	inline std::array flags =
	{
		"Bot",
		"Money",
		"Wins",
		"Rank",
		"Armor",
		"Zoom",
		"C4"
	};

	inline std::array droppedFlags =
	{
		"Box",
		"Text",
		"Icon",
		"Ammo"
	};

	inline std::array straferModes =
	{
		"OFF",
		"Sidespeed",
		"Rotate",
		"Directional"
	};

	inline std::array screenEffects =
	{
		"none",
		"effects/healthboost",
		"effects/parachute",
		"effects/screentear",
		"effects/dronecam",
		"effects/nightvision",
		"effects/dangerzone_screen",
		"effects/underwater_overlay",
		"effects/survival_zone_projection_distancefield_inv",
		"dev/screenhighlight_pulse",
	};

	inline std::array aimbotMethods =
	{
		"Crosshair",
		"Dynamic"
	};

	inline std::array backtrackAim =
	{
		"Last tick",
		"Closest"
	};

	inline std::array beamNames =
	{
		"sprites/blueglow1.vmt",
		"sprites/bubble.vmt",
		"sprites/glow01.vmt",
		"sprites/physbeam.vmt",
		"sprites/purpleglow1.vmt",
		"sprites/purplelaser1.vmt",
		"sprites/radio.vmt",
		"sprites/white.vmt",
	};

	inline std::array aimbotWeapons
	{
		"Riffle",
		"Smg",
		"Machine Gun",
		"Pistol",
		"Deagle",
		"Revolver",
		"Scout",
		"Awp",
		"Auto sniper",
		"Shotgun"
	};
}