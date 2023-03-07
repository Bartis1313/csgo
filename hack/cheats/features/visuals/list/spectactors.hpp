#pragma once

#include <cheats/classes/renderableToPresent.hpp>

#include <unordered_map>
#include <string>
#include <SDK/Enums.hpp>

class SpectactorList : RenderablePresentType
{
public:
	SpectactorList() :
		RenderablePresentType{}
	{}
	
protected:
	virtual void draw() override;
	virtual void reset() override;
private:
	std::unordered_map<ObserverTypes, std::string_view> m_modeString
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

	std::vector<std::pair<std::string, ObserverTypes>> m_specs;
};

GLOBAL_FEATURE(SpectactorList);