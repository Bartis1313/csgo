#pragma once
#include "../../../SDK/structs/Entity.hpp"
#include "../../../SDK/interfaces/interfaces.hpp"
#include "../../../utilities/renderer/renderer.hpp"

struct Box;

// I put other functions in cpp, I only need draw main functions
namespace esp
{
	void run();
	void drawSound(IGameEvent* event);
	void drawInfo(Player_t* ent, const Box& box);
	void drawPlayer(Player_t* ent);

	void drawHealth(Player_t* ent, const Box& box);
	void drawArmor(Player_t* ent, const Box& box);
	void drawWeapon(Player_t* ent, const Box& box);
	void drawSkeleton(Player_t* ent);
	void drawSnapLine(Player_t* ent, const Box& box);

	// 3d
	void renderBox3D(Entity_t* ent, bool fill = false);

	// boxes stuff
	void drawBox2D(Player_t* ent, const Box& box);
	void drawBox2DFilled(Player_t* ent, const Box& box);
	
};