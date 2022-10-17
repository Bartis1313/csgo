#pragma once

#include <dependencies/minhook/Minhook.h>
#include <utilities/tools/tools.hpp>
#include <utilities/console/console.hpp>

namespace hookHelper
{
	template<typename T>
	inline constexpr void** ORIGINAL(T& arg)
	{
		return reinterpret_cast<void**>(&arg);
	}

	inline void tryHook(void* target, void* detour, void* original, const std::string_view name)
	{
		const MH_STATUS hk = MH_CreateHook(static_cast<LPVOID>(target), static_cast<LPVOID>(detour), static_cast<LPVOID*>(original));
		if (hk != MH_OK)
			throw std::runtime_error(FORMAT(XOR("{} hook error"), name));
		LOG_INFO(XOR("{} -> {} hooked at addr 0x{:X}"), name, MH_StatusToString(hk), reinterpret_cast<uintptr_t>(target));
	}

	inline void checkAllHooks()
	{
		const auto status = MH_EnableHook(MH_ALL_HOOKS);
		if (status != MH_OK)
			throw std::runtime_error(XOR("MH_EnableHook error"));
	}

	inline void initMinhook()
	{
		if (MH_Initialize() != MH_OK)
			throw std::runtime_error(XOR("MH_Initialize error"));
	}

	inline void shutdownAllHooks()
	{
		MH_DisableHook(MH_ALL_HOOKS);
		MH_RemoveHook(MH_ALL_HOOKS);
		MH_Uninitialize();
	}
}