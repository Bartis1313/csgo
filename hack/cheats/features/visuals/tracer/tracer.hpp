#pragma once

#include <cheats/classes/renderableToSurface.hpp>
#include <SDK/math/Vector.hpp>

class BulletTracer : protected RenderableSurfaceType
{
public:
	constexpr BulletTracer() :
		RenderableSurfaceType{}
	{}

protected:
	void draw() override;
private:
	struct HitStruct_t
	{
		Vec3 m_pos;
		float m_expire;
		bool called = false;
	};

	std::vector<HitStruct_t> m_hitsClientSide;
};

GLOBAL_FEATURE(BulletTracer);
