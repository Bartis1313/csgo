#pragma once
#include "../../../SDK/structs/Entity.hpp"
#include "../../../SDK/interfaces/interfaces.hpp"
#include "../../../utilities/renderer/renderer.hpp"

namespace world
{
	void drawMisc();
	void drawProjectiles(Entity_t* ent);
	void drawBomb(Entity_t* ent);
	void drawBombDropped(Entity_t* ent);

	void skyboxLoad(int stage);

	// TODO:
	//void drawSmoke(Entity_t* ent);
	void drawMolotovPoints(Entity_t* ent);
	void drawSmokeTimer(Entity_t* ent);
	void drawMolotovTimer(Entity_t* ent);
}