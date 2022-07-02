#pragma once

#include <vector>

#include "../../../SDK/math/Vector.hpp"
#include "../../../SDK/Color.hpp"

class Entity_t;
class Bomb_t;
class IConVar;
class IMaterial;
class CEffectData;

struct hitStructLocal_t
{
	Vector m_start;
	Vector m_end;
	float m_expire;
};

class World final
{
public:
	void drawMisc();

	void skyboxLoad(int stage, bool isShotdown);
	void removeSky(bool isShotdown);
	void modulateWorld(void* thisptr, float* r, float* g, float* b, bool isShutdown);
	void drawZeusRange();
	void drawMovementTrail();
	void clientSideImpacts();
	void localImpacts();
	void pushLocalImpacts(const hitStructLocal_t& hit);
	void removeBloodSpray(int frame);
	void removeSmoke(int frame);
	void drawCustomSmoke(const Vector& pos, float radius);
	void drawBombOverlay();
	Bomb_t* m_bombEnt;
	std::string m_whoPlanted = "";
private:
	void drawProjectiles(Entity_t* ent, const int id);
	void drawBomb(Entity_t* ent);
	void drawMolotov(Entity_t* ent);
	void drawSmoke(Entity_t* ent);
	void resetBomb() { m_bombEnt = nullptr; };
private:
	IConVar* m_oldSky = nullptr;
	struct Trail_t
	{
		Vector m_pos;
		float m_expire;
		Color m_col;
	};
	std::vector<Trail_t> m_trails;
	struct clientHitVerify_t
	{
		Vector m_pos;
		float m_time;
		float m_expire;
	};
	struct hitStruct_t
	{
		Vector m_pos;
		float m_expire;
	};
	std::vector<hitStruct_t> m_hitsClientSide;
	std::vector<hitStructLocal_t> m_hitsLocal;
};

inline World world;