#pragma once

#include <cheats/classes/renderableToSurface.hpp>
#include <SDK/math/Vector.hpp>
#include <render/BBox.hpp>

class IGameEvent;
class Player_t;

class Hitmarker : protected RenderableSurfaceType
{
public:
	constexpr Hitmarker() :
		RenderableSurfaceType{}
	{}

	void handleHits(IGameEvent* event);
protected:
	virtual void draw() override;
private:
	struct Hitmark_t
	{
		float expireTime;
		int damage;
		int health;
		bool wasHeadshot;
		Player_t* player;
		// dont touch those outside of loop
		Vec3 pos;
		float alpha;
	};

	std::vector<Hitmark_t> m_hitmarkers;
};

GLOBAL_FEATURE(Hitmarker);