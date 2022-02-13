#pragma once

namespace GUI
{
	bool isKeyDown(const unsigned key);
	bool isKeyPressed(const unsigned key);
	void raiseKey(const unsigned key);
	void forceKey(const unsigned key);
	bool isMouseInRange(const int x, const int y, const int width, const int height);
	bool isMouseInCircle(const int xCircle, const int yCircle, const int radius);
}