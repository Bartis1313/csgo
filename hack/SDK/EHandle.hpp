#pragma once

#include <cstdint>

struct EHandle_t
{
	uint32_t handle;
	constexpr bool operator!() const { return handle != -1; } // 0xFFFFFFFF
	// when cant use ! at beginning
	constexpr bool isValid() const { return handle != -1; } // 0xFFFFFFFF
};
