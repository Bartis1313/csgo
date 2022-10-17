#include "nadewarn.hpp"

#include <SDK/math/Vector.hpp>
#include <SDK/IWeapon.hpp>
#include <SDK/IVEngineClient.hpp>
#include <SDK/CGlobalVars.hpp>
#include <SDK/IEngineTrace.hpp>
#include <SDK/ICvar.hpp>
#include <SDK/Convar.hpp>
#include <SDK/Enums.hpp>
#include <SDK/CUserCmd.hpp>
#include <SDK/ClientClass.hpp>
#include <SDK/IVModelInfo.hpp>
#include <SDK/IClientEntityList.hpp>
#include <SDK/interfaces/interfaces.hpp>
#include <config/vars.hpp>
#include <game/game.hpp>
#include <game/globals.hpp>
#include <utilities/renderer/renderer.hpp>
#include <utilities/math/math.hpp>
#include <utilities/tools/tools.hpp>
#include <utilities/tools/wrappers.hpp>
#include <utilities/utilities.hpp>
#include <gamememory/memory.hpp>

#include <ranges>

bool NadeTrace_t::step(float interval)
{
	if (m_detonated)
		return true;

	Vec3 move;
	addGravityMove(move, m_velocity, interval);
	Trace_t tr;
	pushEntity(move, tr);

	if (tr.didHit())
		resolveFlyCollisionCustom(tr, interval);

	m_pos = tr.m_end;

	return false;
}

void NadeTrace_t::physicsClipVelocity(const Vec3& in, const Vec3& normal, Vec3& out, float overbounce)
{
	constexpr float STOP_EPSILON = 0.1f;

	float backoff = in.dot(normal) * overbounce;
	for (auto i : std::views::iota(0, 3))
	{
		out[i] = in[i] - (normal[i] * backoff);

		if (out[i] > -STOP_EPSILON && out[i] < STOP_EPSILON)
			out[i] = 0.f;
	}
}

void NadeTrace_t::addGravityMove(Vec3& move, Vec3& vel, float frametime)
{
	move[Coord::X] = vel[Coord::X] * frametime;
	move[Coord::Y] = vel[Coord::Y] * frametime;

	const static float svgrav = interfaces::cvar->findVar(XOR("sv_gravity"))->getFloat();
	float gravity = svgrav * 0.4f;
	float z = vel[Coord::Z] - (gravity * frametime);
	move[Coord::Z] = ((vel[Coord::Z] + z) / 2.0f) * frametime;

	vel[Coord::Z] = z;
}

void NadeTrace_t::traceHull(const Vec3& src, const Vec3& end, Entity_t* entity, Trace_t* tr)
{
	uintptr_t filter[] =
	{
		*reinterpret_cast<uintptr_t*>(g_Memory.m_traceFilterSimple()),
		reinterpret_cast<uintptr_t>(entity),
		0,
		0
	};

	interfaces::trace->traceRay({ src, end, Vec3{ -2.0f, -2.0f, -2.0f }, Vec3{ 2.f, 2.0f, 2.0f } }, MASK_SOLID, reinterpret_cast<TraceFilter*>(&filter), tr);
}

void NadeTrace_t::pushEntity(const Vec3& src, Trace_t& tr)
{
	traceHull(m_pos, m_pos + src, m_nadeOwner, &tr);

	// lazy attempt... see nadepred check code
	if (m_index == WEAPON_MOLOTOV || m_index == WEAPON_INCGRENADE)
	{
		const static float weapon_molotov_maxdetonateslope = interfaces::cvar->findVar(XOR("weapon_molotov_maxdetonateslope"))->getFloat();

		if (bool res = tr.didHit() && tr.m_plane.m_normal[Coord::Z] >= std::cos(math::DEG2RAD(weapon_molotov_maxdetonateslope)); res)
			destroyTrace();
	}
}

void NadeTrace_t::resolveFlyCollisionCustom(Trace_t& tr, float interval)
{
	/*float surfaceElasticity = 1.0;*/
	if (auto e = tr.m_entity; e) // if it's any entity
	{
		if (auto e = tr.m_entity; e->isPlayer()) // if player don't stop but make it bouned a lot slower
		{
			/*surfaceElasticity = 0.3f;*/
			m_velocity *= 0.3f;
			return;
		}

		if (e->isBreakable()) // for example glass or window
		{
			if (!e->isAlive()) // any better solution?
			{
				m_velocity *= 0.4f;
				return;
			}
		}
		// here some checks are needed, I honestly don't know how to make it pixel perfect in this case
	}

	constexpr float surfaceElasticity = 1.0f;
	constexpr float nadeElascity = 0.45f;
	float totalElascity = surfaceElasticity * nadeElascity;
	totalElascity = std::clamp(totalElascity, 0.0f, 0.9f);

	Vec3 absVelocity;
	physicsClipVelocity(m_velocity, tr.m_plane.m_normal, absVelocity, 2.0f);
	absVelocity *= totalElascity;

	constexpr float minSpeed = 20.0f * 20.0f;
	float speedAbsSqr = absVelocity.lengthSqrt();

	if (speedAbsSqr < minSpeed)
		absVelocity = {};

	if (tr.m_plane.m_normal[Coord::Z] > 0.7f)
	{
		m_velocity = absVelocity;
		absVelocity *= ((1.0f - tr.m_fraction) * interval);
		pushEntity(absVelocity * ((1.0f - tr.m_fraction) * interval), tr);
	}
	else
		m_velocity = absVelocity;

	// or velocity
	if (m_bouncesCheck > 20)
		return destroyTrace();

	++m_bouncesCheck;
}

void NadeTrace_t::handleDestroy()
{
	if (m_index == WEAPON_DECOY || m_index == WEAPON_SMOKEGRENADE)
		if (m_velocity.toVecPrev().length() <= 0.1f) // ghetto workaround, at least we can be sure this is accurate
		{
			//printf("did destroy\n");
			destroyTrace();
		}

	if (game::ticksToTime(m_tick) > m_nadeDetonateTime)
		destroyTrace();

	m_nextTick = m_tick + game::timeToTicks(0.2f);
}

void NadeTrace_t::handleDetonates()
{
	switch (m_index)
	{
	case WEAPON_SMOKEGRENADE:
	{
		m_nadeDetonateTime = 3.0f;
		break;
	}
	case WEAPON_DECOY:
	{
		m_nadeDetonateTime = 5.0f;
		break;
	}
	case WEAPON_FLASHBANG:
	case WEAPON_HEGRENADE:
	{
		m_nadeDetonateTime = 1.5f;
		break;
	}
	case WEAPON_MOLOTOV:
	case WEAPON_INCGRENADE:
	{
		const static auto molotov_throw_detonate_time = interfaces::cvar->findVar(XOR("molotov_throw_detonate_time"));
		m_nadeDetonateTime = molotov_throw_detonate_time->getFloat();
		break;
	}
	default:
		m_nadeDetonateTime = 0.0f;
		break;
	}
}

void NadeTrace_t::simulate(const Vec3& pos, const Vec3& velocity, float nadeThrowTime, uint32_t ticks)
{
	m_pos = pos;
	m_velocity = velocity;

	handleDetonates();

	float interval = interfaces::globalVars->m_intervalPerTick;
	for (; m_tick < game::timeToTicks(500.0f); m_tick++) // 500 = 500 * 2 about 1000+
	{
		if (m_nextTick <= m_tick)
			handleDestroy();

		if (m_tick < ticks)
			continue;

		if (step(interval))
			break;

		push();
	}

	m_nadeEndTime = nadeThrowTime + game::ticksToTime(m_tick);
}

void NadeTrace_t::destroyTrace()
{
	m_detonated = true;
}

void NadeTrace_t::push()
{
	m_path.push_back(m_pos);
}

bool NadeTrace_t::draw(Entity_t* entity, WeaponIndex idx)
{
	if (m_path.empty())
		return false;

	ImVec2 start; // need access outside
	if (float dist = m_path.back().distToMeters(game::localPlayer->absOrigin()); dist > vars::misc->nade->tracerDist)
		return false;

	for (Vec3 prev = m_path.front(); const auto & el : m_path)
	{
		if (ImVec2 end; imRender.worldToScreen(prev, start) && imRender.worldToScreen(el, end))
			imRender.drawLine(start, end, vars::misc->nade->colorTracer(), 2.0f);

		prev = el;
	}

	// from visuals, but not gonna use current ent pos for better look
	auto scaledFont = [=](const float division = 80.0f, const float min = 12.0f, const float max = 30.0f)
	{
		float dist = m_path.back().distTo(game::localPlayer->absOrigin());
		float fontSize = std::clamp(division / (dist / division), min, max);
		return fontSize;
	};

	float scale = ((m_nadeEndTime - interfaces::globalVars->m_curtime) / game::ticksToTime(m_tick));
	float rad = scaledFont();

	imRender.drawCircleFilled(start.x, start.y, rad, 32, Colors::Black);
	imRender.drawProgressRing(start.x, start.y, rad, 32, -90, scale, 3.0f, Colors::Green);

	auto name = utilities::u8toStr(reinterpret_cast<Weapon_t*>(entity)->getIcon(idx));
	auto size = ImFonts::icon->CalcTextSizeA(rad + 5.0f, std::numeric_limits<float>::max(), 0.0f, name.c_str());
	imRender.text(start.x, start.y - (size.y / 2.0f), rad + 5.0f, ImFonts::icon, name, true, Colors::White, false);

	auto rotatePoint2D = [](const Vec2& source, const Vec2& dest, float rotateAngle)
	{
		const auto delta = dest - source;

		Vec2 pointRotation
		{
			delta[Coord::X] * std::sin(rotateAngle) - delta[Coord::Y] * std::cos(rotateAngle),
			delta[Coord::X] * std::cos(rotateAngle) + delta[Coord::Y] * std::sin(rotateAngle)
		};

		return source + pointRotation;
	};

	ImVec2 uselessVec;
	if (vars::misc->nade->tracerWarn && !imRender.worldToScreen(m_pos, uselessVec))
	{
		const auto centre = Vec2{ globals::screenX / 2.0f, globals::screenY / 2.0f };

		Vec3 localViewAngle;
		interfaces::engine->getViewAngles(localViewAngle);
		const auto& localPos = game::localPlayer->absOrigin();
		const auto angleToNade = math::calcAngleRelative(localPos, m_pos, localViewAngle);

		auto screenPosition = centre;
		screenPosition[Coord::X] -= std::clamp(localPos.distTo(m_pos), 120.0f, centre[Coord::Y] - 12.0f); // 12.0f - min size possible here so wanna clip it

		const auto pos = rotatePoint2D(centre, screenPosition, math::DEG2RAD(angleToNade[Coord::Y]));

		imRender.drawCircleFilled(pos[Coord::X], pos[Coord::Y], rad, 32, Colors::Black);
		imRender.drawProgressRing(pos[Coord::X], pos[Coord::Y], rad, 32, -90.0f, scale, 3.0f, Colors::Green);

		auto name = utilities::u8toStr(reinterpret_cast<Weapon_t*>(entity)->getIcon(idx));
		auto size = ImFonts::icon->CalcTextSizeA(rad + 5.0f, std::numeric_limits<float>::max(), 0.0f, name.c_str());
		imRender.text(pos[Coord::X], pos[Coord::Y] - (size.y / 2.0f), rad + 5.0f, ImFonts::icon, name, true, Colors::White, false);
	}

	return true;
}

#include "../../cache/cache.hpp"

void GrenadeWarningPaint::draw()
{
	if (!m_datas.empty())
		m_datas.clear();

	if (!vars::misc->nade->enabledTracer)
		return;

	for (auto [entity, idx, classID] : EntityCache::getCache(EntCacheType::GRENADE_PROJECTILES))
	{
		auto ent = reinterpret_cast<Nade_t*>(entity);

		if (ent->isDormant())
			continue;

		auto model = ent->getModel();
		if (!model)
			return;

		auto studio = interfaces::modelInfo->getStudioModel(model);
		if (!studio)
			return;

		auto wpnIdx = game::getNadeByClass(classID, studio);
		if (wpnIdx == WEAPON_NONE)
			return;

		m_datas.emplace(std::make_pair(
			idx,
			NadeTrace_t
			{
				reinterpret_cast<Player_t*>(interfaces::entList->getClientFromHandle(ent->m_hThrower())),
				wpnIdx
			}));

		// simulates, in this place because there is no much math related stuff needed for angles etc
		m_datas.at(idx).simulate(
			ent->m_vecOrigin(),
			reinterpret_cast<Player_t*>(ent)->m_vecVelocity(),
			ent->m_flSpawnTime(),
			game::timeToTicks(reinterpret_cast<Player_t*>(ent)->m_flSimulationTime() - ent->m_flSpawnTime())
		);

		// if no path, then delete this owner from map
		if (!m_datas.at(idx).draw(ent, wpnIdx))
			m_datas.erase(idx);
	}
}