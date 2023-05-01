#pragma once

#include <SDK/math/Vector.hpp>

class CViewSetup;

namespace freecam
{
	void run(CViewSetup* view);
	void updateKeys();

	inline bool isInCam{ false };
};