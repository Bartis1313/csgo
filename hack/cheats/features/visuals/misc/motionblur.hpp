#pragma once

class CViewSetup;

namespace motionBlur
{
	void render();
	void run(CViewSetup* view);

	void initMaterials();
	void drawBlur();
}
