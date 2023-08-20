#include "isHltv.hpp"

#include <SDK/CGlobalVars.hpp>
#include <gamememory/memory.hpp>
#include <config/vars.hpp>

#include <intrin.h>

hooks::IsHltv::value hooks::IsHltv::hook(HACK_FAST_ARGS)
{
	volatile uintptr_t ent{ };
	__asm mov ent, edi

	const retaddr_t ret = reinterpret_cast<retaddr_t>(_ReturnAddress());

	if (!ent)
		return original(thisptr);

	// 53 8B DC 83 EC 08 83 E4 F8 83 C4 04 55 8B 6B 04 89 6C 24 04 8B EC 83 EC 30 56 57 8B F9 8B 0D ? ? ? ? 8B 01 8B 80 ? ? ? ? FF D0 84 C0 0F 85 ? ? ? ?
	if (ret == memory::occlusion() && vars::misc->mirrorCam->enabled)
	{
		*reinterpret_cast<int*>(ent + 0xA24) = -1; // m_nCustomBlendingRuleMask
		*reinterpret_cast<int*>(ent + 0xA30) = memory::interfaces::globalVars->m_frameCount; // m_nComputedLODframe
		*reinterpret_cast<int*>(ent + 0xA2C) = *reinterpret_cast<int*>(ent + 0xA28); // m_nAnimLODflagsOld
		*reinterpret_cast<int*>(ent + 0xA28) = 0; // m_nAnimLODflags

		return true;
	}

	return original(thisptr);
}