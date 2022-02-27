#pragma once
#include "../../../dependencies/minhook/Minhook.h"
#include "../../../utilities/utilities.hpp"

namespace hookHelper
{
	template<typename T>
	inline void** ORIGINAL(T& arg)
	{
		return reinterpret_cast<void**>(&arg);
	}

	inline void tryHook(void* target, void* detour, void* original, const std::string& name)
	{
		const MH_STATUS hk = MH_CreateHook(static_cast<LPVOID>(target), static_cast<LPVOID>(detour), static_cast<LPVOID*>(original));
		if (hk != MH_OK)
			throw std::runtime_error(std::format(XOR("{} hook error"), name));
		LOG(LOG_INFO, std::format(XOR("{} -> {}"), name, MH_StatusToString(hk)));
	}

	inline void checkAllHooks()
	{
		const auto status = MH_EnableHook(MH_ALL_HOOKS);
		if (status != MH_OK)
			throw std::runtime_error(XOR("MH_ALL_HOOKS error"));
	}

	inline void initMinhook()
	{
		if (MH_Initialize() != MH_OK)
			throw std::runtime_error(XOR("MH_Initialize hook error"));
	}

	inline void shutdownAllHooks()
	{
		MH_DisableHook(MH_ALL_HOOKS);
		MH_RemoveHook(MH_ALL_HOOKS);
		MH_Uninitialize();
	}
}