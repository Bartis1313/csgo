#pragma once

#include <classes/renderableToSurface.hpp>
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

class PlayerVisuals : public RenderableSurfaceType
{
public:
	constexpr PlayerVisuals() :
		RenderableSurfaceType{}
	{}

	virtual void init();
	virtual void draw();
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
	void resetDormacy([[maybe_unused]] IGameEvent* event);
	void updateDormacy(Player_t* ent);

	std::array<int, 65> m_health;
	std::array<int, 65> m_armor;

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

[[maybe_unused]] inline auto g_PlayerVisuals = PlayerVisuals{};
