#include "isHltv.hpp"

#include <SDK/CGlobalVars.hpp>
#include <gamememory/memory.hpp>
#include <config/vars.hpp>

hooks::IsHltv::value hooks::IsHltv::hook(FAST_ARGS)
{
	uintptr_t ent;
	__asm mov ent, edi

	const static auto occlusion = memory::occlusion();
	const static auto velocity = memory::velocity();
	const static auto accumulate = memory::accumulate();

	if (memory::retAddr() == occlusion)
	{
		// no need to set those values for normal view
		if (vars::misc->mirrorCam->enabled && ent)
		{
			*reinterpret_cast<int*>(ent + 0xA24) = -1;
			*reinterpret_cast<int*>(ent + 0xA30) = memory::interfaces::globalVars->m_frameCount;
			*reinterpret_cast<int*>(ent + 0xA2C) = *reinterpret_cast<int*>(ent + 0xA28);
			*reinterpret_cast<int*>(ent + 0xA28) = 0;

			return true;
		}
	}

	// skip layers & setup velocity -> or EFL_DIRTY_ABSANGVELOCITY and set direct vel not interpolated
	if (memory::retAddr() == velocity || memory::retAddr() == accumulate)
		return true;
	
	return original(thisptr);
}