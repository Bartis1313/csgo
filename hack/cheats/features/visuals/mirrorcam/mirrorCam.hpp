#pragma once

class CViewSetup;

namespace mirrorcam
{
	void run(const CViewSetup& view);
	void updateKeys();
	void initTexture();
	void draw();
}