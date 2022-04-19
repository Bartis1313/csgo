#pragma once

#include <array>

struct Box;
class IGameEvent;
class Player_t;
class Entity_t;
struct Box3D;
class Color;

class Visuals final
{
public:
	void run();
	void drawSound(IGameEvent* event);
private:
	void drawInfo(Player_t* ent, const Box& box);
	void drawnName(Player_t* ent, const Box& box);
	void drawPlayer(Player_t* ent);
	void runDLight(Player_t* ent);
	void drawHealth(Player_t* ent, const Box& box);
	void drawArmor(Player_t* ent, const Box& box);
	void drawWeapon(Player_t* ent, const Box& box);
	void drawSkeleton(Player_t* ent);
	void drawSnapLine(Player_t* ent, const Box& box);
	void drawLaser(Player_t* ent);
	void enemyIsAimingAtYou(Player_t* ent);
	void drawBox2D(const Box& box);
	void drawBox2DFilled(const Box& box);
	void drawBox3D(const Box3D& box, const float thickness = 2.0f);
	void drawBox3DFilled(const Box3D& box, const float thickness = 2.0f);
public:
	Color healthBased(Player_t* ent);
	// returns the size for any font based on distance and passed division
	float getScaledFontSize(Entity_t* ent, const float division = 80.0f, const float min = 12.0f, const float max = 30.0f);
private:
	std::array<int, 65> m_health;
	std::array<int, 65> m_armor;
};

inline Visuals visuals;