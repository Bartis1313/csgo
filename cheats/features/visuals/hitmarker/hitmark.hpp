#pragma once

#include <classes/renderableToSurface.hpp>
#include <SDK/math/Vector.hpp>

class IGameEvent;

class Hitmarker : protected RenderableSurfaceType
{
public:
	constexpr Hitmarker() :
		RenderableSurfaceType{}
	{}

protected:
	virtual void draw() override;
	virtual void init() override;
private:
	void handleHits(IGameEvent* event);

	struct Hitmark_t
	{
		float m_expire;
		int m_dmg;
		int m_health;
		bool m_head;
		Vec3 m_pos;

		bool isAvailable() const { return m_health >= 0; }
	};
	std::vector<Hitmark_t> m_hitmarkers;
};

GLOBAL_FEATURE(Hitmarker);