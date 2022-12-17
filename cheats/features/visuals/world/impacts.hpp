#pragma once

#include <classes/renderableToSurface.hpp>
#include <classes/onlyInit.hpp>

#include <SDK/math/Vector.hpp>

class IGameEvent;

class BulletImpactsClient : protected RenderableSurfaceType
{
public:
	constexpr BulletImpactsClient() :
		RenderableSurfaceType{}
	{}

protected:
	virtual void draw() override;
private:
	struct HitStruct_t
	{
		Vec3 m_pos;
		float m_expire;
	};

	std::vector<HitStruct_t> m_hitsClientSide;
};

GLOBAL_FEATURE(BulletImpactsClient);
