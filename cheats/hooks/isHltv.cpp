#include "hooks.hpp"

#include <intrin.h>

#include <SDK/CGlobalVars.hpp>
#include <SDK/interfaces/interfaces.hpp>
#include <gamememory/memory.hpp>
#include <config/vars.hpp>

bool __fastcall hooks::isHltv::hooked(FAST_ARGS)
{
	uintptr_t ent;
	__asm mov ent, edi

	const static auto occlusion = g_Memory.m_occlusion();
	const static auto velocity = g_Memory.m_velocity();
	const static auto accumulate = g_Memory.m_accumulate();
	const uintptr_t ret = reinterpret_cast<uintptr_t>(_ReturnAddress());

	if (ret == occlusion)
	{
		// no need to set those values for normal view
		if (vars::misc->mirrorCam->enabled && ent)
		{
			/**(_DWORD*)(ent + 0xA24) = -1;
			*(_DWORD*)(ent + 0xA30) = *(_DWORD*)(globalvarsPTR + 4);
			*(_DWORD*)(ent + 0xA2C) = *(_DWORD*)(ent + 0xA28);
			*(_DWORD*)(ent + 0xA28) = 0;*/

			*reinterpret_cast<int*>(ent + 0xA24) = -1;
			*reinterpret_cast<int*>(ent + 0xA30) = interfaces::globalVars->m_frameCount;
			*reinterpret_cast<int*>(ent + 0xA2C) = *reinterpret_cast<int*>(ent + 0xA28);
			*reinterpret_cast<int*>(ent + 0xA28) = 0;

			return true;
		}
	}

	// skip layers & setup velocity -> or EFL_DIRTY_ABSANGVELOCITY and set direct vel not interpolated
	if (ret == velocity || ret == accumulate)
		return true;
	
	return original(thisptr);
}