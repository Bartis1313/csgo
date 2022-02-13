#include "guimisc.hpp"
#include "../../../globals.hpp"
#include "../../../../SDK/math/Vector2D.hpp"

bool GUI::isKeyDown(const unsigned key)
{
	return globals::keyState[key];
}

bool GUI::isKeyPressed(const unsigned key)
{
	return globals::keyState[key] && !globals::previousKeyState[key];
}

void GUI::raiseKey(const unsigned key)
{
	globals::keyState[key] = false;
}

void GUI::forceKey(const unsigned key)
{
	globals::keyState[key] = true;
}

// do not use any other detections, unless you know math a lot and want big details in this detection
// rectangle detection is enough in most cases

bool GUI::isMouseInRange(const int x, const int y, const int width, const int height)
{
	return globals::mouseX > x && globals::mouseY > y
		&& globals::mouseX < width + x && globals::mouseY < height + y;
}

bool GUI::isMouseInCircle(const int xCircle, const int yCircle, const int radius)
{
	auto distance = [](const Vector2D& posCircle, const Vector2D& posMouse)
	{
		float x = posCircle.x - posMouse.x;
		float y = posCircle.y - posMouse.y;
		float multiplied = x * x + y * y;
		float dist = std::sqrt(multiplied);

		return dist;
	};

	return distance(Vector2D(xCircle, yCircle), Vector2D(globals::mouseX, globals::mouseY)) <= radius;
}