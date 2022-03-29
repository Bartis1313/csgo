#pragma once

struct Box;
class IGameEvent;
class Player_t;
class Entity_t;

class Visuals final
{
public:
	void run();
	void drawSound(IGameEvent* event);
	void drawInfo(Player_t* ent, const Box& box);
	void drawPlayer(Player_t* ent);
	void runDLight(Player_t* ent);
	void drawHealth(Player_t* ent, const Box& box);
	void drawArmor(Player_t* ent, const Box& box);
	void drawWeapon(Player_t* ent, const Box& box);
	void drawSkeleton(Player_t* ent);
	void drawSnapLine(Player_t* ent, const Box& box);
	void drawLaser(Player_t* ent);
	void renderBox3D(Entity_t* ent, bool fill = false);
	void drawBox2D(Player_t* ent, const Box& box);
	void drawBox2DFilled(Player_t* ent, const Box& box);
	void enemyIsAimingAtYou(Player_t* ent);
};

inline Visuals visuals;