#pragma once
#include "../../../SDK/structs/Entity.hpp"
#include "../../../SDK/interfaces/interfaces.hpp"
#include "../../../utilities/renderer/renderer.hpp"

namespace radar
{
	Vector2D entToRadar(Vector eye, Vector angles, Vector EntityPos, Vector2D pos, Vector2D size, float scale, bool& check);
	void run();
}