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
#include <cheats/game/game.hpp>
#include <cheats/game/globals.hpp>
#include <render/render.hpp>
#include <utilities/math/math.hpp>
#include <utilities/tools/tools.hpp>
#include <utilities/tools/wrappers.hpp>
#include <gamememory/memory.hpp>

#include "../../cache/cache.hpp"

#include <ranges>

#include <cheats/hooks/createMove.hpp>
#include <cheats/hooks/paintTraverse.hpp>

namespace
{
	struct NadePredDraw : hooks::PaintTraverse
	{
		NadePredDraw()
		{
			this->registerRender(grenadePrediction::draw);
		}
	} nadeDraw;

	struct NadeButtons : hooks::CreateMove
	{
		NadeButtons()
		{
			this->registerRunPrePrediction(grenadePrediction::run);
		}
	} nadeButtons;
}

#define DETONATE 1
#define BOUNCE 2

namespace grenadePrediction
{
	std::vector<Vec3> pathNade;
	std::vector<Vec3> bouncesNade;
	bool isReady;
	int weaponIndex;
	void setup(Vec3& src, Vec3& vecThrow, const Vec3& viewangles);
	size_t step(Vec3& src, Vec3& vecThrow, int tick, float interval);
	void simulate();
	bool checkDetonate(const Vec3& vecThrow, const Trace_t& tr, int tick, float interval);
	void addGravityMove(Vec3& move, Vec3& vel, float frametime);
	void traceHull(Vec3& src, Vec3& end, Trace_t& tr);
	void pushEntity(Vec3& src, const Vec3& move, Trace_t& tr);
	void resolveFlyCollisionCustom(Trace_t& tr, Vec3& velocity, const Vec3& move, float interval);
	void physicsClipVelocity(const Vec3& in, const Vec3& normal, Vec3& out, float overbounce);
}

void grenadePrediction::run(CUserCmd* cmd)
{
	if (!vars::misc->nade->enabledPred)
		return;

	isReady = false;

	if (vars::misc->nade->predAlways)
		isReady = true;
	else
		isReady = cmd->m_buttons & (IN_ATTACK | IN_ATTACK2);

	auto wpn = game::localPlayer->getActiveWeapon();
	if (!wpn || !wpn->isGrenade())
		return;

	if (isReady)
	{
		weaponIndex = wpn->m_iItemDefinitionIndex();
		simulate();
	}
	else
		weaponIndex = WEAPON_NONE;
}

struct DmgNadeInfo_t
{
	ImVec2 m_pos;
	std::string m_textDmg;
	Color m_color;
};

void grenadePrediction::draw()
{
	if (!vars::misc->nade->enabledPred)
		return;

	if (!game::isAvailable())
		return;

	auto weapon = game::localPlayer->getActiveWeapon();
	if (!weapon)
		return;

	if (!weapon->isGrenade())
		return;

	if (!weaponIndex)
		return;

	if (pathNade.empty())
		return;

	std::vector<DmgNadeInfo_t> nadesDmg = {};

	// valve values
	constexpr float a = 105.0f;
	constexpr float b = 25.0f;
	constexpr float c = 140.0f;

	for (auto [entity, idx, classID] : EntityCache::getCache(EntCacheType::PLAYER))
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

		float deltaDist = (ent->absOrigin() - pathNade.back()).length();

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
			if (ImVec2 pos; ImRender::worldToScreen(ent->absOrigin(), pos))
			{
				std::string text = dmgDealt < 0.0f ? "DIE" : std::format("{:.2f}", -resultDmg);
				nadesDmg.emplace_back(pos, text, Color::healthBased(static_cast<uint8_t>(dmgDealt)));
			}
		}
		else if (idx == WEAPON_MOLOTOV || idx == WEAPON_INCGRENADE)
		{
			if (deltaDist > 180.0f) // 180 is not always accurate perfectly
				continue;

			if (ImVec2 pos; ImRender::worldToScreen(ent->absOrigin(), pos))
				nadesDmg.emplace_back(pos, "In range", Colors::LightBlue);
		}
	}

	std::vector<ImVec2> points;
	for (const auto & el : pathNade)
	{
		if (ImVec2 screen; ImRender::worldToScreen(el, screen))
			points.push_back(screen);
	}
	if (!points.empty())
		ImRender::drawPolyLine(points, vars::misc->nade->colorPredLine(), 0, 2.0f);

	for (const auto& el : bouncesNade)
	{
		ImRender::drawBox3DFilled(el, 2.0f, 2.0f, vars::misc->nade->colorPredBox(), vars::misc->nade->colorPredBoxFill());
	}

	for (const auto& [pos, text, color] : nadesDmg)
	{
		ImRender::text(pos.x, pos.y, ImRender::fonts::tahoma20, text, true, color, false);
	}

	//ImRender::drawCircle3D(m_path.back(), weapon->getNadeRadius(), 32, Colors::White);
}

// not using magic value given by valve, so we never are based on buttons
void grenadePrediction::setup(Vec3& src, Vec3& vecThrow, const Vec3& viewangles)
{
	auto weapon = game::localPlayer->getActiveWeapon();
	if (!weapon)
		return;

	Vec3 angThrow = viewangles;
	float pitch = math::normalizePitch(angThrow[Coord::X]);

	constexpr float inversed = 10.0f / 90.0f;
	angThrow[Coord::X] -= (90.0f - std::abs(angThrow[Coord::X])) * inversed;
	Vec3 forward = math::angleVec(angThrow);

	float m_flThrowStrength = weapon->m_flThrowStrength();
	float clampedVelWeapon = std::min(std::max(weapon->getWpnInfo()->m_throwVelocity * 0.9f, 15.0f), 750.0f);

	float throwHeight = (m_flThrowStrength * 12.0f) - 12.0f;

	src = game::localPlayer->getEyePos();

	float finalVel = clampedVelWeapon * ((0.7f * m_flThrowStrength) + 0.3f);

	Vec3 start = game::localPlayer->getEyePos() += Vec3{ 0.0f, 0.0f, throwHeight };
	Vec3 end = start + (forward * 22.0f);

	Trace_t tr;
	traceHull(src, end, tr);

	Vec3 back = forward;
	back *= 6.0f;
	src = tr.m_end;
	src -= back;

	vecThrow = game::localPlayer->m_vecVelocity();
	vecThrow *= 1.25f;
	vecThrow += forward * finalVel;
}

void grenadePrediction::simulate()
{
	Vec3 vecSrc, vecThrow;
	Vec3 angles; memory::interfaces::engine->getViewAngles(angles);
	setup(vecSrc, vecThrow, angles);

	float interval = memory::interfaces::globalVars->m_intervalPerTick;
	size_t logstep = static_cast<size_t>(0.05f / interval);
	size_t logtimer = 0;

	pathNade.clear();
	bouncesNade.clear();

	for (auto i : std::views::iota(0, 1024)) // 1024 is hardcoded, we prob will never ever reach it
	{
		if (!logtimer)
			pathNade.push_back(vecSrc);

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

	pathNade.push_back(vecSrc);
	bouncesNade.push_back(vecSrc);
}

size_t grenadePrediction::step(Vec3& src, Vec3& vecThrow, int tick, float interval)
{
	Vec3 move;
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
		bouncesNade.push_back(tr.m_end);
	}

	if (auto size = bouncesNade.size() - 1; !bouncesNade.empty() && size > 20)
		bouncesNade.erase(bouncesNade.begin() + size);

	src = tr.m_end;
	return result;
}

bool grenadePrediction::checkDetonate(const Vec3& vecThrow, const Trace_t& tr, int tick, float interval)
{
	auto check = [=](float amount = 0.2f)
	{
		return !(tick % game::timeToTicks(amount));
	};

	const float time = game::ticksToTime(tick);

	switch (weaponIndex)
	{
	case WEAPON_SMOKEGRENADE:
	{
		return vecThrow.toVecPrev().length() <= 0.1f && check();
	}
	case WEAPON_DECOY:
	{
		return vecThrow.toVecPrev().length() <= 0.2f && check();
	}
	case WEAPON_MOLOTOV:
	case WEAPON_INCGRENADE:
	{
		const static float molotov_throw_detonate_time = memory::interfaces::cvar->findVar("molotov_throw_detonate_time")->getFloat();
		const static float weapon_molotov_maxdetonateslope = memory::interfaces::cvar->findVar("weapon_molotov_maxdetonateslope")->getFloat();

		if (tr.didHit() && tr.m_plane.m_normal[Coord::Z] >= std::cos(math::DEG2RAD(weapon_molotov_maxdetonateslope)))
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
void grenadePrediction::traceHull(Vec3& src, Vec3& end, Trace_t& tr)
{
	uintptr_t filter[] =
	{
		*reinterpret_cast<uintptr_t*>(memory::traceFilterSimple()),
		reinterpret_cast<uintptr_t>(game::localPlayer()),
		0,
		0
	};

	memory::interfaces::trace->traceRay({ src, end, Vec3{ -2.0f, -2.0f, -2.0f }, Vec3{ 2.0f, 2.0f, 2.0f } }, MASK_SOLID, reinterpret_cast<TraceFilter*>(&filter), &tr);
}

void grenadePrediction::addGravityMove(Vec3& move, Vec3& vel, float frametime)
{
	move[Coord::X] = vel[Coord::X] * frametime;
	move[Coord::Y] = vel[Coord::Y] * frametime;

	const static float svgrav = memory::interfaces::cvar->findVar("sv_gravity")->getFloat();
	float gravity = svgrav * 0.4f;
	float z = vel[Coord::Z] - (gravity * frametime);
	move[Coord::Z] = ((vel[Coord::Z] + z) / 2.0f) * frametime;

	vel[Coord::Z] = z;
}

void grenadePrediction::pushEntity(Vec3& src, const Vec3& move, Trace_t& tr)
{
	Vec3 end = src;
	end += move;
	traceHull(src, end, tr);
}

inline bool fClassnameIs(Entity_t* ent, const std::string_view name)
{
	if (!ent)
		return false;

	auto cl = ent->clientClass();
	if (!cl)
		return false;

	return std::string_view{ cl->m_networkName } == name;
}

void grenadePrediction::resolveFlyCollisionCustom(Trace_t& tr, Vec3& velocity, const Vec3& move, float interval)
{
	// better fix isbroken, temp solution
	if (tr.m_entity->isPlayer() && tr.m_entity->m_iHealth() > 0)
	{
		velocity *= 0.4f;
		//tr.m_entity->setAbsVelocity(velocity);

		return;
	}

	float surfaceElasticity = 1.0f;
	if (tr.m_entity->isPlayer())
		surfaceElasticity = 0.3f;

	constexpr float nadeElascity = 0.45f;
	float totalElascity = surfaceElasticity * nadeElascity;
	totalElascity = std::clamp(totalElascity, 0.0f, 0.9f);

	Vec3 absVelocity;
	physicsClipVelocity(velocity, tr.m_plane.m_normal, absVelocity, 2.0f);
	absVelocity *= totalElascity;

	constexpr float minSpeed = 20.0f * 20.0f;
	float speedAbsSqr = absVelocity.lengthSqrt();

	if (speedAbsSqr < minSpeed)
		absVelocity = {};

	if (tr.m_plane.m_normal[Coord::Z] > 0.7f)
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

void grenadePrediction::physicsClipVelocity(const Vec3& in, const Vec3& normal, Vec3& out, float overbounce)
{
	constexpr float STOP_EPSILON = 0.1f;

	float backoff = in.dot(normal) * overbounce;
	for (auto i : std::views::iota(0, 3))
	{
		out[i] = in[i] - (normal[i] * backoff);

		if (out[i] > -STOP_EPSILON && out[i] < STOP_EPSILON)
			out[i] = 0.0f;
	}
}
