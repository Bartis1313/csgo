#pragma once

namespace vfunc
{
	inline constexpr void* getVFunc(void* thisptr, size_t index)
	{
		return (*static_cast<void***>(thisptr))[index];
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
// type - type of function
// name - function's name
// index - vtable index for this method
// args - args to pass eg: (int value)
// variables - variables from args, the starting will always be a pointer to class, eg: (this, value)
#define VFUNC(type, name, index, args, variables) \
type name args { \
	return vfunc::callVFunc<type, index>variables; \
}