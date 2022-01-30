#pragma once

namespace GUI
{
	bool isKeyDown(const short key);
	bool isKeyPressed(const short key);
	void raiseKey(const short key);
	void forceKey(const short key);
	bool isMouseInRange(const int x, const int y, const int width, const int height);
	bool isMouseInCircle(const int xCircle, const int yCircle, const int radius);
}