#include "nadepred.hpp"

#include "../../../SDK/math/Vector.hpp"
#include "../../../SDK/IWeapon.hpp"
#include "../../../SDK/IVEngineClient.hpp"
#include "../../../SDK/CGlobalVars.hpp"
#include "../../../SDK/IEngineTrace.hpp"
#include "../../../SDK/ICvar.hpp"
#include "../../../SDK/Convar.hpp"
#include "../../../SDK/Enums.hpp"

#include "../../../config/vars.hpp"
#include "../../game.hpp"
#include "../../globals.hpp"
#include "../../../utilities/renderer/renderer.hpp"
#include "../../../utilities/math/math.hpp"

#define DETONATE 1
#define BOUNCE 2

enum ACT
{
	ACT_NONE,
	ACT_THROW,
	ACT_LOB,
	ACT_DROP,
};

void GrenadePrediction::createMove(int buttons)
{
	if (!config.get<bool>(vars.bNadePred))
		return;

	if (config.get<bool>(vars.bNadePredAlways))
	{
		m_act = ACT_THROW;
		return;
	}

	m_act = ACT_NONE;

	int attack = buttons & IN_ATTACK;
	int attack2 = buttons & IN_ATTACK2;

	if (attack || attack2)
	{
		if (attack && attack2)
			m_act = ACT_LOB;
		else if (!attack)
			m_act = ACT_DROP;
		else
			m_act = ACT_THROW;
	}
}

void GrenadePrediction::viewSetup()
{
	if (!config.get<bool>(vars.bNadePred))
		return;

	if (!game::isAvailable())
		return;

	auto wpn = game::localPlayer->getActiveWeapon();
	if (!wpn || !wpn->isGrenade())
		return;

	if (m_act != ACT_NONE)
	{
		m_indexWpn = wpn->m_iItemDefinitionIndex();
		simulate();
	}
	else
		m_indexWpn = 0;
}

void GrenadePrediction::draw()
{
	if (!config.get<bool>(vars.bNadePred))
		return;

	if (!game::isAvailable())
		return;

	auto weapon = game::localPlayer->getActiveWeapon();
	if (!weapon)
		return;

	if (!weapon->isGrenade())
		return;

	if (!m_indexWpn)
		return;

	if (m_path.size() < 2)
		return;

	std::vector<ImVec2> pollys = {}; // debug needs to change settings or smth, makes this vector gone
	for (const auto& el : m_path)
	{
		if (Vector2D screen; imRender.worldToScreen(el, screen))
			pollys.emplace_back(ImVec2{ screen.x, screen.y });
	}

	if (!pollys.empty())
		imRender.drawPolyLine(pollys.size(), pollys.data(), Colors::LightBlue, false, 2.0f);

	for (const auto& el : m_bounces)
	{
		imRender.drawBox3DFilled(el, { 4.0f, 4.0f }, 4.0f, Colors::Green, Colors::Green);
	}

	imRender.drawCircle3D(m_path.back(), weapon->getNadeRadius(), 32, Colors::White);
}

// not using magic value given by valve, so we never are based on buttons
void GrenadePrediction::setup(Vector& src, Vector& vecThrow, const Vector& viewangles)
{
	auto weapon = game::localPlayer->getActiveWeapon();
	if (!weapon)
		return;

	Vector angThrow = viewangles;
	float pitch = math::normalizePitch(angThrow.x);

	constexpr float inversed = 10.0f / 90.0f;
	angThrow.x -= (90.0f - std::abs(angThrow.x)) * inversed;
	Vector forward = math::angleVec(angThrow);

	float m_flThrowStrength = weapon->m_flThrowStrength();
	float clampedVelWeapon = std::min(std::max(weapon->getWpnInfo()->m_throwVelocity * 0.9f, 15.0f), 750.0f);

	float throwHeight = (m_flThrowStrength * 12.0f) - 12.0f;

	src = game::localPlayer->getEyePos();

	float finalVel = clampedVelWeapon * ((0.7f * m_flThrowStrength) + 0.3f);

	Vector start = game::localPlayer->getEyePos() += { 0.0f, 0.0f, throwHeight };
	Vector end = start + (forward * 22.0f);

	Trace_t tr;
	traceHull(src, end, tr);

	Vector back = forward;
	back *= 6.0f;
	src = tr.m_end;
	src -= back;

	vecThrow = game::localPlayer->m_vecVelocity();
	vecThrow *= 1.25f;
	vecThrow += forward * finalVel;
}

void GrenadePrediction::simulate()
{
	Vector vecSrc, vecThrow;
	Vector angles; interfaces::engine->getViewAngles(angles);
	setup(vecSrc, vecThrow, angles);

	float interval = interfaces::globalVars->m_intervalPerTick;
	int logstep = static_cast<int>(0.05f / interval);
	size_t logtimer = 0;

	m_path.clear();
	m_bounces.clear();

	for (size_t i = 0; i < 1024; ++i) // 1024 is hardcoded, we prob will never ever reach it
	{
		if (!logtimer)
			m_path.push_back(vecSrc);

		size_t s = step(vecSrc, vecThrow, i, interval);

		if (s & DETONATE)
			break;

		if (s & BOUNCE || logtimer >= logstep)
			logtimer = 0;
		else
			++logtimer;

		if (vecThrow.isZero())
			break;
	}

	m_path.push_back(vecSrc);
	m_bounces.push_back(vecSrc);
}

size_t GrenadePrediction::step(Vector& src, Vector& vecThrow, int tick, float interval)
{
	Vector move;
	addGravityMove(move, vecThrow, interval);
	Trace_t tr;
	pushEntity(src, move, tr);

	size_t result = 0;

	if (checkDetonate(vecThrow, tr, tick, interval))
		result |= DETONATE;

	if (tr.didHit())
	{
		result |= BOUNCE;
		resolveFlyCollisionCustom(tr, vecThrow, move, interval);
		m_bounces.push_back(tr.m_end);
	}

	src = tr.m_end;
	return result;
}

bool GrenadePrediction::checkDetonate(const Vector& vecThrow, const Trace_t& tr, int tick, float interval)
{
	auto check = [=](float amount = 0.2f)
	{
		return !(tick % static_cast<int>(amount / interval));
	};

	const float time = interval * tick;

	switch (m_indexWpn)
	{
	case WEAPON_SMOKEGRENADE:
	{
		if (vecThrow.length2D() <= 0.1f)
			return check();

		return false;
	}
	case WEAPON_DECOY:
	{
		if (vecThrow.length2D() <= 0.2f)
			return check();

		return false;
	}
	case WEAPON_MOLOTOV:
	case WEAPON_FIREBOMB:
	{
		const static float molotov_throw_detonate_time = interfaces::cvar->findVar(XOR("molotov_throw_detonate_time"))->getFloat();
		const static float weapon_molotov_maxdetonateslope = interfaces::cvar->findVar(XOR("weapon_molotov_maxdetonateslope"))->getFloat();

		if (tr.didHit() && tr.m_plane.m_normal.z >= std::cos(DEG2RAD(weapon_molotov_maxdetonateslope)))
			return true;

		return time >= molotov_throw_detonate_time && check();
	}
	case WEAPON_FLASHBANG:
	case WEAPON_HEGRENADE:
		return time >= 1.5f && check();
	default:
		return false;
	}

	return false;
}

// should add more traces, as in some cases the prediction can fail badly
void GrenadePrediction::traceHull(Vector& src, Vector& end, Trace_t& tr)
{
	TraceFilter filter{ game::localPlayer };
	Ray_t ray{ src, end, { -2.0f, -2.0f, -2.0f }, { 2.0f, 2.0f, 2.0f } };
	interfaces::trace->traceRay(ray, MASK_SOLID, &filter, &tr);
}

void GrenadePrediction::addGravityMove(Vector& move, Vector& vel, float frametime)
{
	move.x = vel.x * frametime;
	move.y = vel.y * frametime;

	const static float svgrav = interfaces::cvar->findVar(XOR("sv_gravity"))->getFloat();
	float gravity = svgrav * 0.4f;
	float z = vel.z - (gravity * frametime);
	move.z = ((vel.z + z) / 2.0f) * frametime;

	vel.z = z;
}

void GrenadePrediction::pushEntity(Vector& src, const Vector& move, Trace_t& tr)
{
	Vector end = src;
	end += move;
	traceHull(src, end, tr);
}

void GrenadePrediction::resolveFlyCollisionCustom(Trace_t& tr, Vector& velocity, const Vector& move, float interval)
{
	/*float surfaceElasticity = 1.0;*/
	if (auto e = tr.m_entity; e) // if it's any entity
	{
		if (auto e = tr.m_entity; e->isPlayer()) // if player don't stop but make it bouned a lot slower
		{
			/*surfaceElasticity = 0.3f;*/
			velocity *= 0.3f;
			return;
		}

		if (e->isBreakable()) // for example glass or window
		{
			if (!e->isAlive()) // any better solution?
			{
				velocity *= 0.4f;
				return;
			}
		}
		// here some checks are needed, I honestly don't know how to make it pixel perfect in this case
	}

	constexpr float surfaceElasticity = 1.0f;
	constexpr float nadeElascity = 0.45f;
	static float totalElascity = surfaceElasticity * nadeElascity;
	totalElascity = std::clamp(totalElascity, 0.0f, 0.9f);

	Vector absVelocity;
	physicsClipVelocity(velocity, tr.m_plane.m_normal, absVelocity, 2.0f);
	absVelocity *= totalElascity;

	if (tr.m_plane.m_normal.z > 0.7f)
	{
		constexpr float minSpeed = 20.0f * 20.0f;
		float speedAbsSqr = absVelocity.lengthSqrt();

		if (speedAbsSqr > 96000.0f)
		{
			float len = absVelocity.normalized().dot(tr.m_plane.m_normal);
			if (len > 0.5f)
				absVelocity *= 1.f - len + 0.5f;
		}

		if (speedAbsSqr < minSpeed)
			absVelocity = {};

		
			velocity = absVelocity;
			pushEntity(tr.m_end, absVelocity * ((1.0f - tr.m_fraction) * interval), tr);
		
	}
	else
	{
		velocity = absVelocity;
		pushEntity(tr.m_end, absVelocity * ((1.0f - tr.m_fraction) * interval), tr);
	}
}

void GrenadePrediction::physicsClipVelocity(const Vector& in, const Vector& normal, Vector& out, float overbounce)
{
	constexpr float STOP_EPSILON = 0.1f;

	float backoff = in.dot(normal) * overbounce;
	for (int i = 0; i < 3; i++)
	{
		out[i] = in[i] - (normal[i] * backoff);

		if (out[i] > -STOP_EPSILON && out[i] < STOP_EPSILON)
			out[i] = 0.f;
	}
}