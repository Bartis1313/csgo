#pragma once
#include <array>
#include "../../../utilities/utilities.hpp"

namespace sections
{
	inline std::array aimbotHitboxes =
	{
		"NEAREST",
		"HEAD",
		"CHEST"
	};

	inline std::array espNames =
	{
		"2D BOX",
		"2D FILLED",
		"3D BOX",
		"3D FILLED"
	};

	inline std::array crossHairNames =
	{
		"OFF",
		"Static",
		"Recoil",
		"Engine"
	};

	inline std::array chamsNames =
	{
		"OFF",
		"Static",
		"Xyz"
	};

	inline std::array btChams =
	{
		"OFF",
		"Stable",
		"Last tick",
		"Rainbow"
	};
}