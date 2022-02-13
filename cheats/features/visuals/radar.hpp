#pragma once
#include "../../../utilities/renderer/renderer.hpp"

namespace radar
{
	Vector2D entToRadar(const Vector& eye, const Vector& angles, const Vector& entPos, const Vector2D& pos, const Vector2D& size, const float scale);
	void run();
}