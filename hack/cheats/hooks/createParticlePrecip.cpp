#include "createParticlePrecip.hpp"

#if UNLOCK_PRECIP_TESTING == true

#include <SDK/interfaces/interfaces.hpp>
#include <SDK/structs/Entity.hpp>
#include <SDK/IVModelInfo.hpp>
#include <SDK/CUtlVector.hpp>

#include <gamememory/memory.hpp>

#include <SDK/structs/Entity.hpp>

hooks::CreateParticlePrecip::value hooks::CreateParticlePrecip::hook(FAST_ARGS)
{
	bool m_bParticlePrecipInitialized = *(bool*)((uintptr_t)thisptr + 0xAA1);

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
#endif