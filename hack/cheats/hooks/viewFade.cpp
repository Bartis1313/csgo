#include "viewFade.hpp"

#include <gamememory/memory.hpp>
#include <config/vars.hpp>

hooks::ViewFade::value hooks::ViewFade::hook(FAST_ARGS, uint8_t* color, IMaterial* material, bool t)
{
	hooks::capture::engineRenderer = thisptr;

	const auto retAddr = reinterpret_cast<retaddr_t>(_ReturnAddress());

	/*if (retAddr == memory::viewFadeSmokeRet())
	{
		return;
	}*/

	if (retAddr == memory::flashbangRet() || retAddr == memory::flashbangWhiteRet())
	{
		// R, G, B go go in 0 255 range, where A stays 255 always
		for (int i = 0; i < 4; ++i)
			color[i] = static_cast<uint8_t>(color[i] * vars::misc->disableItems->flashPercentage);
	}

	original(thisptr, color, material, t);
}