#pragma once

#include <cheats/classes/renderableToSurface.hpp>
#include <SDK/math/Vector.hpp>
#include <SDK/math/Vector.hpp>

#include <array>
#include <deque>

struct Box;
class IGameEvent;
class Player_t;
class Entity_t;
struct Box3D;
class Color;

class PlayerVisuals : protected RenderableSurfaceType
{
public:
	constexpr PlayerVisuals() :
		RenderableSurfaceType{}
	{}

protected:
	virtual void draw() override;
	virtual void roundRestart() override;
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
	void updateDormacy(Player_t* ent);

	std::array<float, 65> m_health;
	std::array<float, 65> m_armor;

	struct DormacyInfo_t
	{
		float m_alpha;
		Vec3 m_lastPos;
		float m_lastUpdate;
		bool m_calledEvent;

		bool isValid() const;
	};

	// for everything except boxes
	std::array<DormacyInfo_t, 65> m_dormant;
	std::array<float, 65> m_boxAlpha;

	bool m_calledEvent; // for dormacy resets
};

GLOBAL_FEATURE(PlayerVisuals);
