#pragma once
#include <cstdint>

namespace vfunc
{
	// if overflow anyhow use std::int32, mainly used for hooks
	inline uintptr_t getVFunc(void* thisptr, size_t index)
	{
		return static_cast<uintptr_t>((*reinterpret_cast<uintptr_t**>(thisptr))[index]);
	}

	// use for any function that does not represent packed args and can be thiscall
	template <typename T, size_t index, typename ... args_t>
	inline constexpr T callVFunc(void* thisptr, args_t... ca_list)
	{
		using virtualFunction = T(__thiscall***)(void*, args_t...);
		return (*reinterpret_cast<virtualFunction>(thisptr))[index](thisptr, ca_list...);
	}

	// ONLY for packed arguments in base case !!!
	template<typename T, size_t index, typename... args_t>
	inline constexpr T callVPack(void* thisptr, args_t... ca_list)
	{
		using packedVirtualFunction = T(__cdecl***)(void*, args_t...);
		return (*reinterpret_cast<packedVirtualFunction>(thisptr))[index](thisptr, ca_list...);
	}
}

//#define VFUNC(type, name, index, ...) \
//type name { \
//	return vfunc::callVFunc<type, index>(this, __VA_ARGS__); \
//}

// only disadvantage is the fact I can't [[nodiscard]] here because type might be void

#define VFUNC(type, name, index, args, variables) \
type name args { \
    return vfunc::callVFunc<type, index>variables; \
}

// for packed vfunctions you will unfortunately need to make function yourself