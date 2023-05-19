#pragma once

class CViewSetup;

namespace freecam
{
	void run(CViewSetup* view);
	void updateKeys();

	inline bool isInCam{ false };
};