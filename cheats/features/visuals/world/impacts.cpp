#include "impacts.hpp"

#include "../../events/events.hpp"

#include <SDK/CGlobalVars.hpp>
#include <SDK/IEngineTrace.hpp>
#include <SDK/IGameEvent.hpp>
#include <SDK/IClientEntityList.hpp>
#include <SDK/IVEngineClient.hpp>
#include <SDK/interfaces/interfaces.hpp>
#include <game/game.hpp>
#include <game/globals.hpp>
#include <config/vars.hpp>
#include <utilities/renderer/renderer.hpp>
#include <utilities/tools/tools.hpp>

void BulletImpactsClient::draw()
{
	if (!vars::visuals->world->impacts->enabledClient)
		return;

	if (!game::isAvailable())
		return;

	// [8D 8F ? ? ? ? F3 0F 10 84 24 + 0x2]
	auto m_vecBulletVerifyListClient = *reinterpret_cast<CUtlVector<ClientHitVerify_t>*>((uintptr_t)game::localPlayer() + 0x11C50);
	static int gameBulletCount = m_vecBulletVerifyListClient.m_size; // init current count

	for (int i = m_vecBulletVerifyListClient.m_size; i > gameBulletCount; i--) // get current bullets, NOT all
		m_hitsClientSide.emplace_back(HitStruct_t
			{
				m_vecBulletVerifyListClient[i - 1].m_pos,
				interfaces::globalVars->m_curtime + vars::visuals->world->impacts->timeClient
			});

	if (m_vecBulletVerifyListClient.m_size != gameBulletCount)
		gameBulletCount = m_vecBulletVerifyListClient.m_size;

	Color outline = vars::visuals->world->impacts->colorClient();
	Color fill = vars::visuals->world->impacts->colorClientFill();

	for (size_t i = 0; const auto & el : m_hitsClientSide)
	{
		float diff = el.m_expire - interfaces::globalVars->m_curtime;

		if (diff < 0.0f)
		{
			m_hitsClientSide.erase(m_hitsClientSide.begin() + i);
			continue;
		}

		imRender.drawBox3DFilled(el.m_pos, 4.0f, 4.0f, outline, fill);

		i++;
	}
}

void BulletImpactsLocal::init()
{
	g_Events->add(XOR("bullet_impact"), std::bind(&BulletImpactsLocal::pushBullet, this, std::placeholders::_1));
}

void BulletImpactsLocal::draw()
{
	if (!vars::visuals->world->impacts->enabledLocal)
		return;

	Color outline = vars::visuals->world->impacts->colorLocal();
	Color fill = vars::visuals->world->impacts->colorLocalFill();

	for (size_t i = 0; const auto & el : m_hitsLocal)
	{
		float diff = el.m_expire - interfaces::globalVars->m_curtime;

		if (diff < 0.0f)
		{
			m_hitsLocal.erase(m_hitsLocal.begin() + i);
			continue;
		}

		Trace_t tr;
		TraceFilter filter;
		filter.m_skip = game::localPlayer();
		interfaces::trace->traceRay({ el.m_start, el.m_end }, MASK_PLAYER, &filter, &tr);

		imRender.drawBox3DFilled(tr.m_end, 4.0f, 4.0f, outline, fill);

		i++;
	}
}

void BulletImpactsLocal::pushBullet(IGameEvent* event)
{
	if (!vars::visuals->world->impacts->enabledLocal)
		return;

	auto attacker = interfaces::entList->getClientEntity(interfaces::engine->getPlayerID(event->getInt(XOR("userid"))));
	if (!attacker)
		return;

	if (attacker != game::localPlayer)
		return;

	auto local = reinterpret_cast<Player_t*>(attacker);

	float x = event->getFloat(XOR("x"));
	float y = event->getFloat(XOR("y"));
	float z = event->getFloat(XOR("z"));

	Vec3 src = local->getEyePos();
	Vec3 dst = Vec3{ x, y, z };

	HitStructLocal_t hit = { src, dst, interfaces::globalVars->m_curtime + vars::visuals->world->impacts->timeLocal };
	m_hitsLocal.push_back(hit);
}
