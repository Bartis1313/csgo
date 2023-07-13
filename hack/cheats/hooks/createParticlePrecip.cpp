#include "createParticlePrecip.hpp"

#include <SDK/interfaces/interfaces.hpp>
#include <SDK/structs/Entity.hpp>
#include <SDK/IVModelInfo.hpp>
#include <SDK/CUtlVector.hpp>

#include <gamememory/memory.hpp>

#include <SDK/structs/Entity.hpp>
#include <cheats/features/visuals/world/weather.hpp>

hooks::CreateParticlePrecip::value hooks::CreateParticlePrecip::hook(FAST_ARGS)
{
	auto precip = reinterpret_cast<Precipitation_t*>(thisptr);
	bool m_bParticlePrecipInitialized = precip->m_bParticlePrecipInitialized();

	if (!m_bParticlePrecipInitialized)
	{
		hooks::calledInitprecip = true;
	}
	else
	{
		hooks::calledInitprecip = false;
	}

	original(thisptr);
}