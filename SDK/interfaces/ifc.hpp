#pragma once

#include <string_view>
#include <gamememory/modules.hpp>

template<typename T>
struct Interface
{
	using value = T;

	T base{ };
	std::string_view module{ CLIENT_DLL };

	[[nodiscard]] constexpr T operator->() const
	{ 
		return base;
	}

	[[nodiscard]] constexpr T operator()() const
	{
		return base;
	}
};