#pragma once

#include <utility>

class Player_t;

namespace enemyWarning
{
	void draw();
	void beginThink(Player_t* ent);
	void endThink();
}
