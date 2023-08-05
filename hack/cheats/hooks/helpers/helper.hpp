#pragma once

#include <Minhook.h>
#include <gamememory/memory.hpp>

#include <utilities/tools/tools.hpp>
#include <utilities/console/console.hpp>

#include <vector>

namespace hookHelper
{
	template<typename T>
	inline constexpr void** ORIGINAL(T& arg)
	{
		return reinterpret_cast<void**>(&arg);
	}

	namespace MinHook
	{
		inline void tryHook(void* target, void* detour, void* original, const std::string_view name)
		{
			const MH_STATUS hk = MH_CreateHook(static_cast<LPVOID>(target), static_cast<LPVOID>(detour), static_cast<LPVOID*>(original));
			if (hk != MH_OK)
				HACK_THROW(std::format("{} hook error", name));
			console::debug("{} -> {} hooked at addr 0x{:X}", name, MH_StatusToString(hk), reinterpret_cast<uintptr_t>(target));

			const MH_STATUS enable = MH_EnableHook(static_cast<LPVOID>(target));
			if(enable != MH_OK)
				HACK_THROW(std::format("{} hook enable error", name));
		}

		inline void initMinhook()
		{
			const MH_STATUS status = MH_Initialize();
			if(status != MH_OK)
				HACK_THROW("Can't init MinHook");

			console::debug("Minhook init -> {}", MH_StatusToString(status));
		}

		inline void shutdownAllHooks()
		{
			MH_DisableHook(MH_ALL_HOOKS);
			MH_RemoveHook(MH_ALL_HOOKS);
			MH_Uninitialize();

			// no need to log
		}
	}

	namespace Valve
	{
		namespace detail
		{
			inline std::vector<std::pair<void*, std::string_view>> allHooks;
		}

		inline void tryHook(void* target, void* detour, void* original, const std::string_view name)
		{
			const bool status = memory::valveHook()(target, detour, original, 0);
			if(!status)
				HACK_THROW(std::format("{} hook error", name));
			console::debug("{} -> {} hooked at addr 0x{:X}", name, status, reinterpret_cast<uintptr_t>(target));

			detail::allHooks.emplace_back(std::make_pair(target, name));
		}

		inline void tryUnhook(void* target, const std::string_view name)
		{
			const bool status = memory::valveUnHook()(reinterpret_cast<uintptr_t>(target), 0);
			if (!status)
				HACK_THROW(std::format("can't unhook! {}", name));
		}

		inline void shutdownAllHooks()
		{
			for (const auto& [target, name] : detail::allHooks)
			{
				tryUnhook(target, name);
			}
		}
	}
}