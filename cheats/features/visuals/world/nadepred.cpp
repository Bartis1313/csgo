#include "nadepred.hpp"

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
#include <gamememory/memory.hpp>

#include "../../cache/cache.hpp"

#include <ranges>

#define DETONATE 1
#define BOUNCE 2

enum ACT
{
	ACT_NONE,
	ACT_THROW,
	ACT_LOB,
	ACT_DROP,
};

void GreandePredictionButton::init()
{

}

void GreandePredictionButton::run(CUserCmd* cmd)
{
	if (!config.get<bool>(vars.bNadePred))
		return;

	if (config.get<bool>(vars.bNadePredAlways))
	{
		m_button = ACT_THROW;
		return;
	}

	m_button = ACT_NONE;

	int attack = cmd->m_buttons & IN_ATTACK;
	int attack2 = cmd->m_buttons & IN_ATTACK2;

	if (attack || attack2)
	{
		if (attack && attack2)
			m_button = ACT_LOB;
		else if (!attack)
			m_button = ACT_DROP;
		else
			m_button = ACT_THROW;
	}

	runView();
}

void GreandePredictionButton::runView()
{
	if (!config.get<bool>(vars.bNadePred))
		return;

	if (!game::isAvailable())
		return;

	auto wpn = game::localPlayer->getActiveWeapon();
	if (!wpn || !wpn->isGrenade())
		return;

	if (m_button != ACT_NONE)
	{
		m_weaponIdx = wpn->m_iItemDefinitionIndex();
		g_GrenadePrediction.simulate();
	}
	else
		m_weaponIdx = 0;
}

struct DmgNadeInfo_t
{
public:
	constexpr DmgNadeInfo_t(const Vector2D& pos, const std::string& dmgtext, const Color& color)
		: m_pos{ pos }, m_textDmg{ dmgtext }, m_color{ color }
	{}

	Vector2D m_pos;
	std::string m_textDmg;
	Color m_color;
};


void GrenadePrediction::init()
{

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

	if (!g_GrenadePredictionButton.getWeaponIdx())
		return;

	if (m_path.empty())
		return;

	std::vector<DmgNadeInfo_t> nadesDmg = {};

	// valve values
	constexpr float a = 105.0f;
	constexpr float b = 25.0f;
	constexpr float c = 140.0f;

	for (auto [entity, idx, classID] : g_EntCache.getCache(EntCacheType::PLAYER))
	{
		auto ent = reinterpret_cast<Player_t*>(entity);
		if (!ent)
			continue;

		if (ent->isDormant())
			continue;

		if (!ent->isAlive())
			continue;

		if (!ent->isOtherTeam(game::localPlayer()))
			continue;

		float deltaDist = (ent->absOrigin() - m_path.back()).length();

		if (int idx = weapon->m_iItemDefinitionIndex(); idx == WEAPON_HEGRENADE)
		{
			if (deltaDist > 350.0f) // nade always same
				continue;

			float d = ((deltaDist - b) / c);
			float dmg = a * std::exp(-d * d);
			float resultDmg = game::scaleDamageArmor(dmg, static_cast<float>(ent->m_ArmorValue()));
			if (resultDmg < 0.1f)
				continue;

			float dmgDealt = ent->m_iHealth() - resultDmg;
			if (Vector2D pos; imRender.worldToScreen(ent->absOrigin(), pos))
			{
				std::string text = dmgDealt < 0.0f ? XOR("DIE") : FORMAT(XOR("{:.2f}"), -resultDmg);
				nadesDmg.emplace_back(pos, text, Color::healthBased(static_cast<uint8_t>(dmgDealt)));
			}
		}
		else if (idx == WEAPON_MOLOTOV || idx == WEAPON_INCGRENADE)
		{
			if (deltaDist > 180.0f) // 180 is not always accurate perfectly
				continue;

			if (Vector2D pos; imRender.worldToScreen(ent->absOrigin(), pos))
				nadesDmg.emplace_back(pos, XOR("In range"), Colors::LightBlue);
		}
	}

	for (Vector prev = m_path.front(); const auto & el : m_path)
	{
		if (ImVec2 start, end; imRender.worldToScreen(prev, start) && imRender.worldToScreen(el, end))
			imRender.drawLine(start, end, config.get<CfgColor>(vars.cNadePredColor).getColor(), 2.0f);

		prev = el;
	}

	for (const auto& el : m_bounces)
	{
		imRender.drawBox3DFilled(el, { 2.0f, 2.0f }, 2.0f, config.get<CfgColor>(vars.cNadeBoxColorOutline).getColor(), config.get<CfgColor>(vars.cNadeBoxColorFill).getColor());
	}

	for (const auto& [pos, text, color] : nadesDmg)
	{
		imRender.text(pos.x, pos.y, ImFonts::tahoma20, text, true, color, false);
	}

	//imRender.drawCircle3D(m_path.back(), weapon->getNadeRadius(), 32, Colors::White);
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
	size_t logstep = static_cast<size_t>(0.05f / interval);
	size_t logtimer = 0;

	m_path.clear();
	m_bounces.clear();

	for (auto i : std::views::iota(0, 1024)) // 1024 is hardcoded, we prob will never ever reach it
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

	if (auto size = m_bounces.size() - 1; !m_bounces.empty() && size > 20)
		m_bounces.erase(m_bounces.begin() + size);

	src = tr.m_end;
	return result;
}

bool GrenadePrediction::checkDetonate(const Vector& vecThrow, const Trace_t& tr, int tick, float interval)
{
	auto check = [=](float amount = 0.2f)
	{
		return !(tick % game::timeToTicks(amount));
	};

	const float time = game::ticksToTime(tick);

	switch (g_GrenadePredictionButton.getWeaponIdx())
	{
	case WEAPON_SMOKEGRENADE:
	{
		return vecThrow.length2D() <= 0.1f && check();
	}
	case WEAPON_DECOY:
	{
		return vecThrow.length2D() <= 0.2f && check();
	}
	case WEAPON_MOLOTOV:
	case WEAPON_INCGRENADE:
	{
		const static float molotov_throw_detonate_time = interfaces::cvar->findVar(XOR("molotov_throw_detonate_time"))->getFloat();
		const static float weapon_molotov_maxdetonateslope = interfaces::cvar->findVar(XOR("weapon_molotov_maxdetonateslope"))->getFloat();

		if (tr.didHit() && tr.m_plane.m_normal.z >= std::cos(math::DEG2RAD(weapon_molotov_maxdetonateslope)))
			return true;

		return time >= molotov_throw_detonate_time && check(0.1f);
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
	uintptr_t filter[] =
	{
		*reinterpret_cast<uintptr_t*>(g_Memory.m_traceFilterSimple()),
		reinterpret_cast<uintptr_t>(game::localPlayer()),
		0,
		0
	};

	interfaces::trace->traceRay({ src, end, { -2.0f, -2.0f, -2.0f }, { 2.0f, 2.0f, 2.0f } }, MASK_SOLID, reinterpret_cast<TraceFilter*>(&filter), &tr);
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

	constexpr float minSpeed = 20.0f * 20.0f;
	float speedAbsSqr = absVelocity.lengthSqrt();

	if (speedAbsSqr < minSpeed)
		absVelocity = {};

	if (tr.m_plane.m_normal.z > 0.7f)
	{
		velocity = absVelocity;
		absVelocity *= ((1.0f - tr.m_fraction) * interval);
		pushEntity(tr.m_end, absVelocity * ((1.0f - tr.m_fraction) * interval), tr);
	}
	else
	{
		velocity = absVelocity;
	}
}

void GrenadePrediction::physicsClipVelocity(const Vector& in, const Vector& normal, Vector& out, float overbounce)
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
