#include "newParticleEffectSetControlPoint.hpp"

#include <cheats/features/visuals/world/weather/precipitation.hpp>
#include <cheats/game/game.hpp>
#include <utilities/math/math.hpp>
#include <SDK/CNewParticleEffect.hpp>
#include <SDK/CCommonHostState.hpp>

#include <config/vars.hpp>

size_t tempCallCounter = 0;

hooks::NewParticleEffectSetControlPoint::value hooks::NewParticleEffectSetControlPoint::hook(FAST_ARGS, int whichPoint, const Vec3& vec)
{
	const auto precip = weather::precipitation::getPrecip();
	if (!precip)
		return original(thisptr, whichPoint, vec);

	if (whichPoint != 1)
		return original(thisptr, whichPoint, vec);

	const auto particleEffect = reinterpret_cast<CNewParticleEffect*>(thisptr);
	const auto collection = particleEffect->getParticleCollection();

	bool detectedWeather = false;
	if (collection)
	{
		const std::string_view name = collection->m_def.m_obj->m_name.m_buffer;
		if (name.contains("rain") || name.contains("snow"))
		{
			detectedWeather = true;
		}
	}

	// dont care, will be same
	// bool outer = false;
	bool innerFar = false;
	bool innerNear = false;

	if (detectedWeather)
	{
		++tempCallCounter;

		if (tempCallCounter == 2)
		{
			innerNear = true;
		}
		else if (tempCallCounter == 3)
		{
			innerFar = true;
		}
		else if (tempCallCounter > 3)
			tempCallCounter = 0;
	}

	if (innerNear)
	{
		const auto forward = math::angleVec(game::getViewAngles());
		const Vec3 customPos = game::localPlayer->getEyePos() + Vec3{ 0.0f, 0.0f, 180.0f } + forward * vars::visuals->world->weather->customNear;
		return original(thisptr, whichPoint, std::cref(customPos));
	}

	if (innerFar)
	{
		const auto forward = math::angleVec(game::getViewAngles());
		const Vec3 customPos = game::localPlayer->getEyePos() + Vec3{ 0.0f, 0.0f, 180.0f } + forward * vars::visuals->world->weather->customFar;
		return original(thisptr, whichPoint, std::cref(customPos));
	}

	original(thisptr, whichPoint, vec);
}