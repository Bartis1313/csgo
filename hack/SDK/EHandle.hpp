#pragma once

#include <cstdint>

struct CBaseHandle
{
	uint32_t handle;
	constexpr bool operator!() const { return handle != -1; } // 0xFFFFFFFF
	// when cant use ! at beginning
	constexpr bool isValid() const { return handle != -1; } // 0xFFFFFFFF
};
