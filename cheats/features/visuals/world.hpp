#pragma once

class Entity_t;
class IConVar;
class IMaterial;

class World final
{
public:
	void drawMisc();
	void drawProjectiles(Entity_t* ent);
	void drawBomb(Entity_t* ent);
	void drawBombDropped(Entity_t* ent);

	void skyboxLoad(int stage, bool isShotdown);
	void removeSky(bool isShotdown);
	void modulateWorld(void* thisptr, void* edx, float* r, float* g, float* b, bool isShutdown);

	// TODO:
	//void drawSmoke(Entity_t* ent);
	void drawMolotovPoints(Entity_t* ent);
	void drawSmokeTimer(Entity_t* ent);
	void drawMolotovTimer(Entity_t* ent);
	void drawZeusRange();

	void drawMovementTrail();
private:
	IConVar* m_oldSky = nullptr;
};

inline World world;