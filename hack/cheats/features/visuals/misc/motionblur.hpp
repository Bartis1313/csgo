#pragma once

#include <SDK/math/Vector.hpp>

#include <array>

class CViewSetup;
class IMaterial;
class ITexture;

namespace motionBlur
{
	void render();
	void run(CViewSetup* view);

	void initMaterials();
	void drawBlur();
}
