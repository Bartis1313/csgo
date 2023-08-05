#pragma once

#include <SDK/IVModelInfo.hpp>
#include <SDK/vars.hpp>

#include "helper.hpp"

enum FrameStage;
struct vcollide_t;
class IGameEvent;
class Precipitation_t;
class DynamicLight_t;

namespace weather::precipitation
{
	void init();
	void run(FrameStage stage);
	void reset();
	void shutdown();
	void preacheCustomFiles();
	std::string_view pickSoundForPrecip(helper::CustomPrecipitationType_t precip);

	Precipitation_t* getPrecip();
	vcollide_t& getCollide();

	constexpr int uniqueModelIndex{ MAX_MODELS - 1 };
}