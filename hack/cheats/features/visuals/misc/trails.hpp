#pragma once

#include <cheats/classes/renderableToSurface.hpp>
#include <SDK/math/Vector.hpp>
#include <render/Color.hpp>

class Trails : protected RenderableSurfaceType
{
public:
	constexpr Trails() :
		RenderableSurfaceType{}
	{}

protected:
	virtual void draw() override;
private:
	struct Trail_t
	{
		Vec3 m_pos;
		float m_expire;
		Color m_col;
	};

	std::vector<Trail_t> m_trails;
};

GLOBAL_FEATURE(Trails);
