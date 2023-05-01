#pragma once

#include <SDK/math/Vector.hpp>
#include <render/BBox.hpp>

class IGameEvent;
class Player_t;

namespace hitmarker
{
	void handleHits(IGameEvent* event);
	void draw();

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

	inline std::vector<Hitmark_t> m_hitmarkers;
}