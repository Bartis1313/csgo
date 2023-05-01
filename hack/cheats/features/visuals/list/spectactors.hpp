#pragma once

#include <unordered_map>
#include <string>
#include <SDK/Enums.hpp>

namespace spectactor
{
	void draw();
	void reset();

	inline std::unordered_map<ObserverTypes, std::string_view> m_modeString
	{
		{ OBS_MODE_NONE, "None" },
		{ OBS_MODE_DEATHCAM, "Deathcam" },
		{ OBS_MODE_FREEZECAM, "Freezecam" },
		{ OBS_MODE_FIXED, "Fixed" },
		{ OBS_MODE_IN_EYE, "In-eye" },
		{ OBS_MODE_CHASE, "Chase" },
		{ OBS_MODE_POI, "Poi" },
		{ OBS_MODE_ROAMING, "Roaming" }
	};

	inline std::vector<std::pair<std::string, ObserverTypes>> m_specs;
}