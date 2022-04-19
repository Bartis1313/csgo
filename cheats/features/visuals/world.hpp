#pragma once

#include <vector>

#include "../../../SDK/math/Vector.hpp"
#include "../../../SDK/Color.hpp"

class Entity_t;
class IConVar;
class IMaterial;

class World final
{
public:
	void drawMisc();

	void skyboxLoad(int stage, bool isShotdown);
	void removeSky(bool isShotdown);
	void modulateWorld(void* thisptr, float* r, float* g, float* b, bool isShutdown);

	void drawZeusRange();
	void drawMovementTrail();

private:
	void drawProjectiles(Entity_t* ent, const int id);
	void drawBomb(Entity_t* ent);
	void drawBombDropped(Entity_t* ent);
	void drawMolotov(Entity_t* ent);
	void drawSmoke(Entity_t* ent);
private:
	IConVar* m_oldSky = nullptr;
	struct Trail_t
	{
		Vector m_pos;
		float m_expire;
		Color m_col;
	};
	std::vector<Trail_t> m_trails;
};

inline World world;