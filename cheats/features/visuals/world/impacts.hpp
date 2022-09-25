#pragma once

#include <classes/renderableToSurface.hpp>
#include <classes/onlyInit.hpp>

#include <SDK/math/Vector.hpp>

class IGameEvent;

class BulletImpactsClient : public RenderableSurfaceType
{
public:
	constexpr BulletImpactsClient() :
		RenderableSurfaceType{}
	{}

	virtual void init();
	virtual void draw();
private:
	struct ClientHitVerify_t
	{
		Vector m_pos;
		float m_time;
		float m_expire;
	};

	struct HitStruct_t
	{
		Vector m_pos;
		float m_expire;
	};

	std::vector<HitStruct_t> m_hitsClientSide;
};

[[maybe_unused]] inline auto g_BulletImpactsClient = BulletImpactsClient{};

class BulletImpactsLocal : RenderableSurfaceType
{
public:
	constexpr BulletImpactsLocal() :
		RenderableSurfaceType{}
	{}

	virtual void init();
	virtual void draw();
private:
	void pushBullet(IGameEvent* event);
	struct HitStructLocal_t
	{
		Vector m_start;
		Vector m_end;
		float m_expire;
	};

	std::vector<HitStructLocal_t> m_hitsLocal;
};

[[maybe_unused]] inline auto g_BulletImpactsLocal = BulletImpactsLocal{};
