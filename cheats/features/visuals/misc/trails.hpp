#pragma once

#include <classes/renderableToSurface.hpp>
#include <SDK/math/Vector.hpp>
#include <SDK/Color.hpp>

class Trails : RenderableSurfaceType
{
public:
	constexpr Trails() :
		RenderableSurfaceType{}
	{}

	virtual void init();
	virtual void draw();
private:
	struct Trail_t
	{
		Vector m_pos;
		float m_expire;
		Color m_col;
	};

	std::vector<Trail_t> m_trails;
};

[[maybe_unused]] inline auto g_Trails = Trails{};
