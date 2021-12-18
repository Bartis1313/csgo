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
	template <typename T, size_t index, typename ... Args_t>
	inline constexpr T callVFunc(void* thisptr, Args_t... args)
	{
		using virtualFunction = T(__thiscall***)(void*, Args_t...);
		return (*reinterpret_cast<virtualFunction>(thisptr))[index](thisptr, args...);
	}

	// ONLY for packed arguments in base case !!!
	template<typename T, size_t index, typename... Args_t>
	inline constexpr T callVPack(void* thisptr, Args_t... args)
	{
		using packedVirtualFunction = T(__cdecl***)(void*, Args_t...);
		return (*reinterpret_cast<packedVirtualFunction>(thisptr))[index](thisptr, args...);
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