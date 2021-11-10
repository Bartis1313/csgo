#pragma once
#include "../../../SDK/structs/Entity.hpp"
#include "../../../SDK/interfaces/interfaces.hpp"
#include "../../../utilities/renderer/renderer.hpp"

struct Box;

// I put other functions in cpp, I only need draw main functions
namespace Esp
{
	void draw();
	void drawMisc();
	void drawSound(IGameEvent* event);
	void drawInfo(Player_t* ent, const Box& box);
	void drawPlayer(Player_t* ent);
	void drawCrosshair();
	void drawProjectiles(Entity_t* ent);
	void drawBomb(Entity_t* ent);
	void drawBombDropped(Entity_t* ent);

	void drawHealth(Player_t* ent, const Box& box);
	void drawArmor(Player_t* ent, const Box& box);
	void drawWeapon(Player_t* ent, const Box& box);
	void drawSkeleton(Player_t* ent);

	// TODO:
	void drawSmoke(Entity_t* ent);
	void drawMolotovPoints(Entity_t* ent);
	void drawSmokeTimer(Entity_t* ent);
	void drawMolotovTimer(Entity_t* ent);

	Vector2D entToRadar(Vector eye, Vector angles, Vector EntityPos, Vector2D pos, Vector2D size, float scale, bool& check);
	void radar();
	
	void drawLocalInfo();

	// helpers
	bool isNade(const int classid);
	// 2d
	bool getBox(Entity_t* ent, Box& box);
	// 3d
	void renderBox3D(Entity_t* ent, bool fill = false);

	// boxes stuff
	void drawBox2D(Player_t* ent, const Box& box);
	void drawBox2DFilled(Player_t* ent, const Box& box);
	


};