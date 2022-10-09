#pragma once

#include <array>

namespace selections
{
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
}