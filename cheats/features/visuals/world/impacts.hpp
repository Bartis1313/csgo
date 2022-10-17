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
	struct ClientHitVerify_t
	{
		Vec3 m_pos;
		float m_time;
		float m_expire;
	};

	struct HitStruct_t
	{
		Vec3 m_pos;
		float m_expire;
	};

	std::vector<HitStruct_t> m_hitsClientSide;
};

GLOBAL_FEATURE(BulletImpactsClient);

class BulletImpactsLocal : protected RenderableSurfaceType
{
public:
	constexpr BulletImpactsLocal() :
		RenderableSurfaceType{}
	{}

protected:
	virtual void draw() override;
	virtual void init() override;
	virtual void reset() override {};
	virtual void shutdown() override {};
private:
	void pushBullet(IGameEvent* event);
	struct HitStructLocal_t
	{
		Vec3 m_start;
		Vec3 m_end;
		float m_expire;
	};

	std::vector<HitStructLocal_t> m_hitsLocal;
};

GLOBAL_FEATURE(BulletImpactsLocal);
