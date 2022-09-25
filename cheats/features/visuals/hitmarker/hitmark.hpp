#pragma once

#include <classes/renderableToSurface.hpp>
#include <SDK/math/Vector.hpp>

class IGameEvent;

class Hitmarker : public RenderableSurfaceType
{
public:
	constexpr Hitmarker() :
		RenderableSurfaceType{}
	{}

	virtual void init();
	virtual void draw();

private:
	void handleHits(IGameEvent* event);

	struct Hitmark_t
	{
		float m_expire;
		int m_dmg;
		int m_health;
		bool m_head;
		Vector m_pos;

		bool isAvailable() const { return m_health >= 0; }
	};
	std::vector<Hitmark_t> m_hitmarkers;
};

[[maybe_unused]] inline auto g_Hitmarker = Hitmarker{};
