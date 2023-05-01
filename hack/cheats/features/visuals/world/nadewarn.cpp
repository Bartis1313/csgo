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
#include <cheats/game/game.hpp>
#include <cheats/game/globals.hpp>
#include <render/render.hpp>
#include <utilities/math/math.hpp>
#include <utilities/tools/tools.hpp>
#include <utilities/tools/wrappers.hpp>
#include <utilities/utilities.hpp>
#include <gamememory/memory.hpp>

#include <ranges>

#include <cheats/hooks/paintTraverse.hpp>

namespace
{
	struct NadeWarningHandler : hooks::PaintTraverse
	{
		NadeWarningHandler()
		{
			this->registerRender(grenadeWarning::draw);
		}
	} nadeWarningHandler;
}

// code not fully made by me
// Uses much less math than predicting the full throw due to existance of networkable values
class NadeTrace_t
{
public:
	NadeTrace_t() = default;
	constexpr NadeTrace_t(Player_t* owner, WeaponIndex index) :
		nadeOwner{ owner }, weaponIndex{ index }
	{}

	void simulate(const Vec3& pos, const Vec3& velocity, float nadeThrowTime, uint32_t ticks);
	bool draw(WeaponIndex idx);
private:
	void addGravityMove(Vec3& move, Vec3& vel, float frametime);
	void physicsClipVelocity(const Vec3& in, const Vec3& normal, Vec3& out, float overbounce);
	void destroyTrace();
	void push();
	void handleDetonates();
	void handleDestroy();
	bool step(float interval);
	void traceHull(const Vec3& src, const Vec3& end, Entity_t* entity, Trace_t* tr);
	void pushEntity(const Vec3& src, Trace_t& tr);
	void resolveFlyCollisionCustom(Trace_t& tr, float interval);
	std::optional<std::string_view> getConsoleName(WeaponIndex idx) const;

	bool m_detonated{ false };
	// nade index
	WeaponIndex weaponIndex{ };
	// current tick for nade loop
	uint32_t nadeTick{ };
	// next tick, based on passed addon, to make this more perfect with tick timing
	uint32_t nextNadeTick{ };
	// for decoy
	size_t bouncesNadeCount{ 0 };

	Player_t* nadeOwner{ };
	Vec3 pos{ };
	Vec3 velocity{ };
	float nadeDetonateTime{ };
	float nadeEndTime{ };
	std::vector<Vec3> nadePath{ };
};

namespace grenadeWarning
{
	std::unordered_map<int, NadeTrace_t> nadeTraces;
}

bool NadeTrace_t::step(float interval)
{
	if (m_detonated)
		return true;

	Vec3 move;
	addGravityMove(move, velocity, interval);
	Trace_t tr;
	pushEntity(move, tr);

	if (tr.didHit())
		resolveFlyCollisionCustom(tr, interval);

	pos = tr.m_end;

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

	const static float svgrav = memory::interfaces::cvar->findVar("sv_gravity")->getFloat();
	float gravity = svgrav * 0.4f;
	float z = vel[Coord::Z] - (gravity * frametime);
	move[Coord::Z] = ((vel[Coord::Z] + z) / 2.0f) * frametime;

	vel[Coord::Z] = z;
}

void NadeTrace_t::traceHull(const Vec3& src, const Vec3& end, Entity_t* entity, Trace_t* tr)
{
	uintptr_t filter[] =
	{
		*reinterpret_cast<uintptr_t*>(memory::traceFilterSimple()),
		reinterpret_cast<uintptr_t>(entity),
		0,
		0
	};

	memory::interfaces::trace->traceRay({ src, end, Vec3{ -2.0f, -2.0f, -2.0f }, Vec3{ 2.f, 2.0f, 2.0f } }, MASK_SOLID, reinterpret_cast<TraceFilter*>(&filter), tr);
}

void NadeTrace_t::pushEntity(const Vec3& src, Trace_t& tr)
{
	traceHull(pos, pos + src, nadeOwner, &tr);

	// lazy attempt... see nadepred check code
	if (weaponIndex == WEAPON_MOLOTOV || weaponIndex == WEAPON_INCGRENADE)
	{
		const static float weapon_molotov_maxdetonateslope = memory::interfaces::cvar->findVar("weapon_molotov_maxdetonateslope")->getFloat();

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
		pushEntity(absVelocity * ((1.0f - tr.m_fraction) * interval), tr);
	}
	else
		velocity = absVelocity;

	// or velocity
	if (bouncesNadeCount > 20)
		return destroyTrace();

	++bouncesNadeCount;
}

void NadeTrace_t::handleDestroy()
{
	if (weaponIndex == WEAPON_DECOY || weaponIndex == WEAPON_SMOKEGRENADE)
		if (velocity.toVecPrev().length() <= 0.1f) // ghetto workaround, at least we can be sure this is accurate
		{
			//printf("did destroy\n");
			destroyTrace();
		}

	if (game::ticksToTime(nadeTick) > nadeDetonateTime)
		destroyTrace();

	nextNadeTick = nadeTick + game::timeToTicks(0.2f);
}

void NadeTrace_t::handleDetonates()
{
	switch (weaponIndex)
	{
	case WEAPON_SMOKEGRENADE:
	{
		nadeDetonateTime = 3.0f;
		break;
	}
	case WEAPON_DECOY:
	{
		nadeDetonateTime = 5.0f;
		break;
	}
	case WEAPON_FLASHBANG:
	case WEAPON_HEGRENADE:
	{
		nadeDetonateTime = 1.5f;
		break;
	}
	case WEAPON_MOLOTOV:
	case WEAPON_INCGRENADE:
	{
		const static auto molotov_throw_detonate_time = memory::interfaces::cvar->findVar("molotov_throw_detonate_time");
		nadeDetonateTime = molotov_throw_detonate_time->getFloat();
		break;
	}
	default:
		nadeDetonateTime = 0.0f;
		break;
	}
}

void NadeTrace_t::simulate(const Vec3& _pos, const Vec3& _velocity, float nadeThrowTime, uint32_t ticks)
{
	pos = _pos;
	velocity = _velocity;

	handleDetonates();

	float interval = memory::interfaces::globalVars->m_intervalPerTick;
	for (; nadeTick < game::timeToTicks(500.0f); nadeTick++) // 500 = 500 * 2 about 1000+
	{
		if (nextNadeTick <= nadeTick)
			handleDestroy();

		if (nadeTick < ticks)
			continue;

		if (step(interval))
			break;

		push();
	}

	nadeEndTime = nadeThrowTime + game::ticksToTime(nadeTick);
}

void NadeTrace_t::destroyTrace()
{
	m_detonated = true;
}

void NadeTrace_t::push()
{
	nadePath.push_back(pos);
}

bool NadeTrace_t::draw(WeaponIndex idx)
{
	if (nadePath.empty())
		return false;

	ImVec2 start; // need access outside
	if (float dist = nadePath.back().distToMeters(game::localPlayer->absOrigin()); dist > vars::misc->nade->tracerDist)
		return false;

	std::vector<ImVec2> points;
	for (const auto & el : nadePath)
	{
		if (ImRender::worldToScreen(el, start))
			points.push_back(start);
	}
	if (!points.empty())
		ImRender::drawPolyLine(points, vars::misc->nade->colorTracer(), 0, 2.0f);

	const float scale = ((nadeEndTime - memory::interfaces::globalVars->m_curtime) / game::ticksToTime(nadeTick));
	const float rad = game::getScaledFont(nadePath.back(), game::localPlayer->absOrigin());

	ImRender::drawCircleFilled(start.x, start.y, rad, 32, Colors::Black);
	ImRender::drawProgressRing(start.x, start.y, rad, 32, -90, scale, 3.0f, Colors::Green);


	if (const auto maybeName = getConsoleName(idx); maybeName.has_value())
	{
		const auto icon = game::getWeaponIcon(maybeName.value());
		const auto sizeIcon = ImVec2{ rad + 2.0f, rad + 2.0f };
		ImRender::drawImage(icon.texture, ImVec2{ start.x - (sizeIcon.y / 2.0f), start.y - (sizeIcon.y / 2.0f) }, sizeIcon, Colors::White);

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
		if (vars::misc->nade->tracerWarn && !ImRender::worldToScreen(pos, uselessVec))
		{
			const auto centre = Vec2{ globals::screenX / 2.0f, globals::screenY / 2.0f };

			Vec3 localViewAngle;
			memory::interfaces::engine->getViewAngles(localViewAngle);
			const auto& localPos = game::localPlayer->absOrigin();
			const auto angleToNade = math::calcAngleRelative(localPos, pos, localViewAngle);

			auto screenPosition = centre;
			screenPosition[Coord::X] -= std::clamp(localPos.distTo(pos), 120.0f, centre[Coord::Y] - 12.0f); // 12.0f - min size possible here so wanna clip it

			const auto pos = rotatePoint2D(centre, screenPosition, math::DEG2RAD(angleToNade[Coord::Y]));

			ImRender::drawCircleFilled(pos[Coord::X], pos[Coord::Y], rad, 32, Colors::Black);
			ImRender::drawProgressRing(pos[Coord::X], pos[Coord::Y], rad, 32, -90.0f, scale, 3.0f, Colors::Green);
			ImRender::drawImage(icon.texture, ImVec2{ pos[Coord::X] - (sizeIcon.x / 2), pos[Coord::Y] - (sizeIcon.y / 2.0f) }, sizeIcon, Colors::White);
		}
	}

	return true;
}

#include "../../cache/cache.hpp"

void grenadeWarning::draw()
{
	if (!nadeTraces.empty())
		nadeTraces.clear();

	if (!vars::misc->nade->enabledTracer)
		return;

	for (auto [entity, idx, classID] : EntityCache::getCache(EntCacheType::GRENADE_PROJECTILES))
	{
		auto ent = reinterpret_cast<Nade_t*>(entity);

		if (ent->isDormant())
			continue;

		auto model = ent->getModel();
		if (!model)
			continue;

		auto studio = memory::interfaces::modelInfo->getStudioModel(model);
		if (!studio)
			continue;

		auto wpnIdx = game::getNadeByClass(classID, studio);
		if (wpnIdx == WEAPON_NONE)
			continue;

		nadeTraces.emplace(std::make_pair(
			idx,
			NadeTrace_t
			{
				reinterpret_cast<Player_t*>(memory::interfaces::entList->getClientFromHandle(ent->m_hThrower())),
				wpnIdx
			}));

		// simulates, in this place because there is no much math related stuff needed for angles etc
		nadeTraces.at(idx).simulate(
			ent->m_vecOrigin(),
			reinterpret_cast<Player_t*>(ent)->m_vecVelocity(),
			ent->m_flSpawnTime(),
			game::timeToTicks(reinterpret_cast<Player_t*>(ent)->m_flSimulationTime() - ent->m_flSpawnTime())
		);

		// if no path, then delete this owner from map
		if (!nadeTraces.at(idx).draw(wpnIdx))
			nadeTraces.erase(idx);
	}
}

std::optional<std::string_view> NadeTrace_t::getConsoleName(WeaponIndex idx) const
{
	static std::unordered_map<WeaponIndex, std::string_view> names =
	{
		{ WEAPON_HEGRENADE, "weapon_hegrenade" },
		{ WEAPON_FLASHBANG, "weapon_flashbang" },
		{ WEAPON_SMOKEGRENADE, "weapon_smokegrenade" },
		{ WEAPON_MOLOTOV, "weapon_molotov" },
		{ WEAPON_INCGRENADE, "weapon_inferno" },
		{ WEAPON_DECOY, "weapon_decoy" }
	};

	if (const auto itr = names.find(idx); itr != names.end())
		return itr->second;

	return std::nullopt;
}