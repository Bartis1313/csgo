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

#include "../../../SDK/ClientClass.hpp"
#include "../../../SDK/IVModelInfo.hpp"
#include "../../../SDK/IClientEntityList.hpp"

#define DETONATE 1
#define BOUNCE 2

enum ACT
{
	ACT_NONE,
	ACT_THROW,
	ACT_LOB,
	ACT_DROP,
};

uint32_t GrenadePrediction::timeToTicks(float time)
{
	return static_cast<uint32_t>(0.5f + time / interfaces::globalVars->m_intervalPerTick);
}

float GrenadePrediction::ticksToTime(float time)
{
	return interfaces::globalVars->m_intervalPerTick * time;
}

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

static Color colorBased(int health)
{
	int g = health * 2.55f;
	int r = 255 - g;
	return Color{ r, g, 0, 255 };
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

	if (m_path.empty())
		return;

	std::vector<DmgNadeInfo_t> nadesDmg = {};

	// valve values
	constexpr float a = 105.0f;
	constexpr float b = 25.0f;
	constexpr float c = 140.0f;

	for (int i = 1; i <= interfaces::globalVars->m_maxClients; i++)
	{
		auto ent = reinterpret_cast<Player_t*>(interfaces::entList->getClientEntity(i));
		if (!ent)
			continue;

		if (ent->isDormant())
			continue;

		if (!ent->isAlive())
			continue;

		if (ent->m_iTeamNum() == game::localPlayer->m_iTeamNum())
			continue;

		float deltaDist = (ent->absOrigin() - m_path.back()).length();
		
		if (int idx = weapon->m_iItemDefinitionIndex(); idx == WEAPON_HEGRENADE)
		{
			if (deltaDist > 350.0f) // nade always same
				continue;

			float d = ((deltaDist - b) / c);
			float dmg = a * std::exp(-d * d);
			float resultDmg = utilities::scaleDamageArmor(dmg, ent->m_ArmorValue());
			if (resultDmg < 0.1f)
				continue;

			float dmgDealt = ent->m_iHealth() - resultDmg;
			if (Vector2D pos; imRender.worldToScreen(ent->absOrigin(), pos))
			{
				std::string text = dmgDealt < 0.0f ? XOR("DIE") : FORMAT(XOR("{:.2f}"), -resultDmg);
				nadesDmg.emplace_back(pos, text, colorBased(dmgDealt));
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

	for (Vector prev = m_path.front(); const auto& el : m_path)
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

	if (tr.m_fraction != 1.0f)
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
		return !(tick % timeToTicks(amount));
	};

	const float time = ticksToTime(tick);

	switch (m_indexWpn)
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

		if (tr.didHit() && tr.m_plane.m_normal.z >= std::cos(DEG2RAD(weapon_molotov_maxdetonateslope)))
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
	const static auto traceFilterSimple = utilities::patternScan(CLIENT_DLL, CTRACE_FILTER_SIMPLE, 0x3D);

	uintptr_t filter[] =
	{
		*reinterpret_cast<uintptr_t*>(traceFilterSimple),
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
	for (int i = 0; i < 3; i++)
	{
		out[i] = in[i] - (normal[i] * backoff);

		if (out[i] > -STOP_EPSILON && out[i] < STOP_EPSILON)
			out[i] = 0.f;
	}
}

bool GrenadeWarning::NadeTrace_t::step(float interval)
{
	if (m_detonated)
		return true;

	Vector move;
	addGravityMove(move, m_velocity, interval);
	Trace_t tr;
	pushEntity(move, tr);

	if (tr.didHit())
		resolveFlyCollisionCustom(tr, interval);

	m_pos = tr.m_end;

	return false;
}

void GrenadeWarning::NadeTrace_t::traceHull(const Vector& src, const Vector& end, Entity_t* entity, Trace_t* tr)
{
	const static auto traceFilterSimple = utilities::patternScan(CLIENT_DLL, CTRACE_FILTER_SIMPLE, 0x3D);

	uintptr_t filter[] =
	{
		*reinterpret_cast<uintptr_t*>(traceFilterSimple),
		reinterpret_cast<uintptr_t>(entity),
		0,
		0
	};

	interfaces::trace->traceRay({ src, end, { -2.0f, -2.0f, -2.0f }, { 2.f, 2.0f, 2.0f } }, MASK_SOLID, reinterpret_cast<TraceFilter*>(&filter), tr);
}

void GrenadeWarning::NadeTrace_t::pushEntity(const Vector& src, Trace_t& tr)
{
	traceHull(m_pos, m_pos + src, m_nadeOwner, &tr);

	// lazy attempt... see nadepred check code
	if (m_index == WEAPON_MOLOTOV || m_index == WEAPON_INCGRENADE)
	{
		const static float weapon_molotov_maxdetonateslope = interfaces::cvar->findVar(XOR("weapon_molotov_maxdetonateslope"))->getFloat();

		if (bool res = tr.didHit() && tr.m_plane.m_normal.z >= std::cos(DEG2RAD(weapon_molotov_maxdetonateslope)); res)
			destroyTrace();
	}
}

void GrenadeWarning::NadeTrace_t::resolveFlyCollisionCustom(Trace_t& tr, float interval)
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

	Vector absVelocity;
	physicsClipVelocity(m_velocity, tr.m_plane.m_normal, absVelocity, 2.0f);
	absVelocity *= totalElascity;

	constexpr float minSpeed = 20.0f * 20.0f;
	float speedAbsSqr = absVelocity.lengthSqrt();

	if (speedAbsSqr < minSpeed)
		absVelocity = {};

	if (tr.m_plane.m_normal.z > 0.7f)
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

void GrenadeWarning::NadeTrace_t::handleDestroy()
{
	if (m_index == WEAPON_DECOY)
		if (m_velocity.length2D() <= 0.1f) // ghetto workaround, at least we can be sure this is accurate
		{
			//printf("did destroy\n");
			destroyTrace();
		}

	if (ticksToTime(m_tick) > m_nadeDetonateTime)
		destroyTrace();

	m_nextTick = m_tick + timeToTicks(0.2f);
}

void GrenadeWarning::NadeTrace_t::handleDetonates()
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

void GrenadeWarning::NadeTrace_t::simulate(const Vector& pos, const Vector& velocity, float nadeThrowTime, int ticks)
{
	m_pos = pos;
	m_velocity = velocity;

	handleDetonates();

	float interval = interfaces::globalVars->m_intervalPerTick;
	for (; m_tick < timeToTicks(500.0f); m_tick++) // 500 = 500 * 2 about 1000+
	{
		if (m_nextTick <= m_tick)
			handleDestroy();

		if (m_tick < ticks)
			continue;

		if (step(interval))
			break;

		push();
	}

	m_nadeEndTime = nadeThrowTime + ticksToTime(m_tick);
}

void GrenadeWarning::NadeTrace_t::destroyTrace()
{
	m_detonated = true;
}

void GrenadeWarning::NadeTrace_t::push()
{
	m_path.push_back(m_pos);
}

WeaponIndex GrenadeWarning::getIndexByClass(int idx, Studiohdr_t* studio)
{
	switch (std::string_view name = studio->m_name; idx)
	{
	case CBaseCSGrenadeProjectile:
	{
		if(name.find(XOR("ggrenade")) != std::string::npos)
			return WEAPON_HEGRENADE;
		else
			return WEAPON_FLASHBANG;
	}
	case CSmokeGrenadeProjectile:
		return WEAPON_SMOKEGRENADE;
	case CMolotovProjectile:
	{
		if (name.find(XOR("molotov")) != std::string::npos)
			return WEAPON_MOLOTOV;
		else
			return WEAPON_INCGRENADE;
	}
	case CDecoyProjectile:
		return WEAPON_DECOY;
	default:
		break;
	}

	return WEAPON_NONE;
}

bool GrenadeWarning::NadeTrace_t::draw(Entity_t* entity, WeaponIndex idx)
{
	if (m_path.empty())
		return false;

	ImVec2 start; // need access outside
	if (float dist = m_path.back().distToMeters(game::localPlayer->absOrigin()); dist > config.get<float>(vars.fNadeTracerMaxDist))
		return false;

	for (Vector prev = m_path.front(); const auto& el : m_path)
	{
		if (ImVec2 end; imRender.worldToScreen(prev, start) && imRender.worldToScreen(el, end))
			imRender.drawLine(start, end, config.get<CfgColor>(vars.cNadeTracer).getColor(), 2.0f);

		prev = el;
	}

	// from visuals, but not gonna use current ent pos for better look
	auto scaledFont = [=](const float division = 80.0f, const float min = 12.0f, const float max = 30.0f)
	{
		float dist = m_path.back().distTo(game::localPlayer->absOrigin());
		float fontSize = std::clamp(division / (dist / division), min, max);
		return fontSize;
	};

	float scale = ((m_nadeEndTime - interfaces::globalVars->m_curtime) / ticksToTime(m_tick));
	float rad = scaledFont();
	
	imRender.drawCircleFilled(start.x, start.y, rad, 32, Colors::Black);
	imRender.drawProgressRing(start.x, start.y, rad, 32, -90, scale, 3.0f, Colors::Green);

	auto name = utilities::u8toStr(reinterpret_cast<Weapon_t*>(entity)->getIcon(idx));
	auto size = ImFonts::icon->CalcTextSizeA(rad + 5.0f, std::numeric_limits<float>::max(), 0.0f, name.c_str());
	imRender.text(start.x, start.y - (size.y / 2.0f), rad + 5.0f, ImFonts::icon, name, true, Colors::White, false);

	auto rotatePoint2D = [](const Vector2D& source, const Vector2D& dest, float rotateAngle)
	{
		const auto delta = dest - source;

		Vector2D pointRotation
		{
			delta.x * std::sin(rotateAngle) - delta.y * std::cos(rotateAngle),
			delta.x * std::cos(rotateAngle) + delta.y * std::sin(rotateAngle)
		};

		return source + pointRotation;
	};

	Vector2D uselessVec;
	if (config.get<bool>(vars.bNadeTracerWarn) && !imRender.worldToScreen(m_pos, uselessVec))
	{
		const auto centre = Vector2D{ globals::screenX / 2.0f, globals::screenY / 2.0f };

		Vector localViewAngle;
		interfaces::engine->getViewAngles(localViewAngle);
		const auto& localPos = game::localPlayer->absOrigin();
		const auto angleToNade = math::calcAngleRelative(localPos, m_pos, localViewAngle);

		auto screenPosition = centre;
		screenPosition.x -= std::clamp(localPos.distTo(m_pos), 120.0f, centre.y - 12.0f); // 12.0f - min size possible here so wanna clip it

		const auto pos = rotatePoint2D(centre, screenPosition, DEG2RAD(angleToNade.y));

		imRender.drawCircleFilled(pos.x, pos.y, rad, 32, Colors::Black);
		imRender.drawProgressRing(pos.x, pos.y, rad, 32, -90.0f, scale, 3.0f, Colors::Green);

		auto name = utilities::u8toStr(reinterpret_cast<Weapon_t*>(entity)->getIcon(idx));
		auto size = ImFonts::icon->CalcTextSizeA(rad + 5.0f, std::numeric_limits<float>::max(), 0.0f, name.c_str()); 
		imRender.text(pos.x, pos.y - (size.y / 2.0f), rad + 5.0f, ImFonts::icon, name, true, Colors::White, false);
	}

	return true;
}

void GrenadeWarning::run(Nade_t* entity)
{
	if (!m_datas.empty())
		m_datas.clear();

	if (!config.get<bool>(vars.bNadeTracer))
		return;

	const auto cl = entity->clientClass();
	if (!cl)
		return;

	int id = cl->m_classID;

	constexpr std::array allowedIDS =
	{
		CBaseCSGrenadeProjectile,
		CSmokeGrenadeProjectile,
		CMolotovProjectile,
		CDecoyProjectile
	};

	// if not found
	if (auto itr = std::find(allowedIDS.cbegin(), allowedIDS.cend(), id); itr == allowedIDS.cend())
		return;

	auto ownerHandle = entity->getIndex();
	// keep it for sure correct, would also place decoy netvar of start but there is none
	if (entity->m_nExplodeEffectTickBegin() ||
		(id == CSmokeGrenadeProjectile && reinterpret_cast<Smoke_t*>(entity)->m_nSmokeEffectTickBegin()))
	{
		m_datas.erase(ownerHandle);
		return;
	}

	auto model = entity->getModel();
	if (!model)
		return;

	auto studio = interfaces::modelInfo->getStudioModel(model);
	if (!studio)
		return;

	auto idx = getIndexByClass(id, studio);
	if (idx == WEAPON_NONE)
		return;

	m_datas.emplace(std::make_pair(
		ownerHandle,
		NadeTrace_t
		{
			reinterpret_cast<Player_t*>(interfaces::entList->getClientFromHandle(entity->m_hThrower())),
			idx
		}));

	// simulates, in this place because there is no much math related stuff needed for angles etc
	m_datas.at(ownerHandle).simulate(
		entity->m_vecOrigin(),
		reinterpret_cast<Player_t*>(entity)->m_vecVelocity(),
		entity->m_flSpawnTime(),
		timeToTicks(reinterpret_cast<Player_t*>(entity)->m_flSimulationTime() - entity->m_flSpawnTime())
	);

	// if no path, then delete this owner from map
	if (!m_datas.at(ownerHandle).draw(entity, idx))
		m_datas.erase(ownerHandle);
}
