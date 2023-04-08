#include "Entity.hpp"

#include "../IVModelInfo.hpp"
#include "../ClientClass.hpp"
#include "../Enums.hpp"
#include "../ICollideable.hpp"
#include "../IClientEntityList.hpp"
#include "../IVEngineClient.hpp"
#include "../CPlayerResource.hpp"
#include "../IEngineTrace.hpp"
#include "../IMDLCache.hpp"
#include "../IWeapon.hpp"
#include "../math/matrix.hpp"
#include "../math/Vector.hpp"
#include "../interfaces/interfaces.hpp"

#include <gamememory/memory.hpp>
#include <utilities/tools/tools.hpp>
#include <utilities/math/math.hpp>

#include <array>

////////////////////////////////////////////////////////////////////////

Vec3 Entity_t::getAimPunch()
{
	Vec3 vec{ };
	vfunc::callVFunc<void, PUNCH>(this, std::ref(vec));
	return vec;
}

Vec3 Entity_t::getEyePos()
{
	Vec3 vec{ };
	vfunc::callVFunc<void, EYE_POS>(this, std::ref(vec));
	return vec;
}

AnimationLayer* Entity_t::getAnimOverlays()
{
	const auto offset = memory::animOverlays();
	return *reinterpret_cast<AnimationLayer**>(uintptr_t(this) + offset);
}

size_t Entity_t::getSequenceActivity(size_t sequence)
{
	const auto studio = memory::interfaces::modelInfo->getStudioModel(this->getModel());
	if (!studio)
		return 0;

	return memory::sequenceActivity()(this, studio, sequence);
}

bool Entity_t::isBreakable()
{
	if (!this)
		return false;

	if (!this->getIndex())
		return false;

	if (const bool res = memory::isBreakable()(this); !res)
		return false;

	const auto cl = this->clientClass();
	if (!cl)
		return false;

	const auto id = cl->m_classID;

	constexpr std::array breakableIds = // for surface ids ONLY!
	{
		CBaseDoor,
		CBreakableSurface,
	};

	if (const auto it = std::find(std::cbegin(breakableIds), std::cend(breakableIds), id); it != breakableIds.cend()) // if id was found from surface
		return true;

	// there we finally check actual entity
	if (id == CBaseEntity && this->collideable()->getSolid() == 1) // can mess with masks, but it's the same
		return true;

	return false;
}

Entity_t* Entity_t::firstMoveChild()
{
	// fill later
	return nullptr;
}

Entity_t* Entity_t::nextMovePeer()
{
	// fill later
	return nullptr;
}

int Entity_t::m_takedamage()
{
	const auto offset = memory::takeDmg();
	return *reinterpret_cast<int*>((uintptr_t)this + offset);
}

void Entity_t::setAbsOrigin(const Vec3& origin)
{
	memory::setAbsOrigin()(this, std::cref(origin));
}

void Entity_t::setAbsVelocity(const Vec3& velocity)
{
	memory::setAbsVelocity()(this, std::cref(velocity));
}

bool Entity_t::setupBonesShort(Matrix3x4* _out, int maxBones, int mask, float time)
{
	__asm
	{
		mov edi, this
		mov eax, [edi + 0x4]
		lea ecx, [edi + 0x4]
		push time
		push mask
		push maxBones
		push _out
		call dword ptr[eax + 0x34]
	}
}

CUtlVector<Matrix3x4> Entity_t::m_CachedBoneData()
{
	const auto offset = memory::cachedBones();
	return *reinterpret_cast<CUtlVector<Matrix3x4>*>(uintptr_t(this) + offset);
}

////////////////////////////////////////////////////////////////////////

std::string Weapon_t::getWpnName()
{
	switch (this->m_iItemDefinitionIndex())
	{
	case WEAPON_DEAGLE:					return "DEAGLE";
	case WEAPON_ELITE:					return "DUAL BERETTAS";
	case WEAPON_FIVESEVEN:				return "FIVE-SEVEN";
	case WEAPON_GLOCK:					return "GLOCK";
	case WEAPON_AK47:					return "AK-47";
	case WEAPON_AUG:					return "AUG";
	case WEAPON_AWP:					return "AWP";
	case WEAPON_FAMAS:					return "FAMAS";
	case WEAPON_G3SG1:					return "G3SG1";
	case WEAPON_GALILAR:				return "GAIL";
	case WEAPON_M249:					return "M249";
	case WEAPON_M4A1:					return "M4A1";
	case WEAPON_MAC10:					return "MAC-10";
	case WEAPON_P90:					return "P90";
	case WEAPON_ZONE_REPULSOR:			return "REPULSOR";
	case WEAPON_MP5SD:					return "MP5";
	case WEAPON_UMP45:					return "UMP-45";
	case WEAPON_XM1014:					return "XM1014";
	case WEAPON_BIZON:					return "PP-BIZON";
	case WEAPON_MAG7:					return "MAG-7";
	case WEAPON_NEGEV:					return "NEGEV";
	case WEAPON_SAWEDOFF:				return "SAWEDOFF";
	case WEAPON_TEC9:					return "TEC-9";
	case WEAPON_TASER:					return "TASER";
	case WEAPON_HKP2000:				return "P2000";
	case WEAPON_MP7:					return "MP7";
	case WEAPON_MP9:					return "MP9";
	case WEAPON_NOVA:					return "NOVA";
	case WEAPON_P250:					return "P250";
	case WEAPON_SHIELD:					return "SHIELD";
	case WEAPON_SCAR20:					return "SCAR-20";
	case WEAPON_SG553:					return "SG-553";
	case WEAPON_SSG08:					return "SCOUT";
	case WEAPON_FLASHBANG:				return "FLASHBANG";
	case WEAPON_HEGRENADE:				return "GRENADE";
	case WEAPON_SMOKEGRENADE:			return "SMOKE";
	case WEAPON_MOLOTOV:				return "MOLOTOV";
	case WEAPON_DECOY:					return "DECOY";
	case WEAPON_INCGRENADE:				return "INC NADE";
	case WEAPON_C4:						return "C4";
	case WEAPON_HEALTHSHOT:				return "HEALTHSHOT";
	case WEAPON_KNIFE_T:				return "KNIFE";
	case WEAPON_M4A1_SILENCER:			return "M4A1-S";
	case WEAPON_USP_SILENCER:			return "USP-S";
	case WEAPON_CZ75A:					return "CZ75-A";
	case WEAPON_REVOLVER:				return "REVOLVER";
	case WEAPON_TAGRENADE:				return "TA GRENADE";
	case WEAPON_FISTS:					return "FISTS";
	case WEAPON_BREACHCHARGE:			return "BREACH CHARGE";
	case WEAPON_TABLET:					return "TABLET";
	case WEAPON_MELEE:					return "KNIFE";
	case WEAPON_AXE:					return "AXE";
	case WEAPON_HAMMER:					return "HAMMER";
	case WEAPON_SPANNER:				return "SPANNER";
	case WEAPON_KNIFE_GHOST:			return "KNIFE";
	case WEAPON_FIREBOMB:				return "FIREBOMB";
	case WEAPON_DIVERSION:				return "DIVERSION";
	case WEAPON_FRAG_GRENADE:			return "FRAG NADE";
	case WEAPON_SNOWBALL:				return "SNOWBALL";
	case WEAPON_BUMPMINE:				return "BUMPMINE";
	case WEAPON_KNIFEGG:
	case WEAPON_KNIFE:
	case WEAPON_KNIFE_BAYONET:
	case WEAPON_KNIFE_CSS:
	case WEAPON_KNIFE_FLIP:
	case WEAPON_KNIFE_GUT:
	case WEAPON_KNIFE_KARAMBIT:
	case WEAPON_KNIFE_M9_BAYONET:
	case WEAPON_KNIFE_TACTICAL:
	case WEAPON_KNIFE_FALCHION:
	case WEAPON_KNIFE_SURVIVAL_BOWIE:
	case WEAPON_KNIFE_BUTTERFLY:
	case WEAPON_KNIFE_PUSH:
	case WEAPON_KNIFE_CORD:
	case WEAPON_KNIFE_CANIS:
	case WEAPON_KNIFE_URSUS:
	case WEAPON_KNIFE_GYPSY_JACKKNIFE:
	case WEAPON_KNIFE_OUTDOOR:
	case WEAPON_KNIFE_STILETTO:
	case WEAPON_KNIFE_WIDOWMAKER:
	case WEAPON_KNIFE_SKELETON:			return "KNIFE";
	default:
		return "Err";
	}
}

bool Weapon_t::isRifle()
{
	if (!this->getWpnInfo())
		return false;

	return this->getWpnInfo()->m_type == WEAPONTYPE_RIFLE;
}

bool Weapon_t::isSmg()
{
	if (!this->getWpnInfo())
		return false;

	return this->getWpnInfo()->m_type == WEAPONTYPE_SUBMACHINEGUN;
}

bool Weapon_t::isMachineGun()
{
	if (!this->getWpnInfo())
		return false;

	return this->getWpnInfo()->m_type == WEAPONTYPE_MACHINEGUN;
}

bool Weapon_t::isShotgun()
{
	if (!this->getWpnInfo())
		return false;

	return this->getWpnInfo()->m_type == WEAPONTYPE_SHOTGUN;
}

bool Weapon_t::isPistol()
{
	if (!this->getWpnInfo())
		return false;

	return this->getWpnInfo()->m_type == WEAPONTYPE_PISTOL;
}

bool Weapon_t::isSniper()
{
	if (!this->getWpnInfo())
		return false;

	return this->getWpnInfo()->m_type == WEAPONTYPE_SNIPER_RIFLE;
}

bool Weapon_t::isGrenade()
{
	if (!this->getWpnInfo())
		return false;

	return this->getWpnInfo()->m_type == WEAPONTYPE_GRENADE;
}

bool Weapon_t::isKnife()
{
	if (!this->getWpnInfo())
		return false;

	return this->getWpnInfo()->m_type == WEAPONTYPE_KNIFE;
}

bool Weapon_t::isNonAimable()
{
	const auto idx = m_iItemDefinitionIndex();

	if (idx == WEAPON_C4 || idx == WEAPON_HEALTHSHOT)
		return true;

	if (isKnife())
		return true;

	if (isGrenade())
		return true;

	return false;
}

size_t Weapon_t::getNadeRadius()
{
	switch (this->m_iItemDefinitionIndex())
	{
	case WEAPON_MOLOTOV:
	case WEAPON_INCGRENADE:
		return 180;
	case WEAPON_SMOKEGRENADE:
		return 144;
	case WEAPON_HEGRENADE:
		return 50;
	default:
		return 10; // because we need to somewhat render it, it's fake
	}
}

/////////////////////////////////////////////////////////////////////////////////

CUserCmd& Player_t::m_LastCmd()
{
	const auto offset = memory::lastCommand();
	return *reinterpret_cast<CUserCmd*>((uintptr_t)this + offset);
}

void Player_t::runThink()
{
	checkHasThinkFunction();
	think();
}

// https://github.com/perilouswithadollarsign/cstrike15_src/blob/f82112a2388b841d72cb62ca48ab1846dfcc11c8/game/client/c_baseplayer.cpp#L2667
void Player_t::postThink()
{
//#if !defined( NO_ENTITY_PREDICTION )
	MDLCACHE_CRITICAL_SECTION(memory::interfaces::mdlCache());

	if (isAlive())
	{
		updateCollisionBounds();

		//if (!CommentaryModeShouldSwallowInput(this))
		//{
		//	// do weapon stuff
		//	ItemPostFrame();
		//}

		if (m_fFlags() & FL_ONGROUND)
		{
			m_flFallVelocity() = 0;
		}

		// Don't allow bogus sequence on player
		if (m_nSequence() == -1)
		{
			setSequence(0);
		}

		studioFrameAdvance();
		memory::postThinkPhysics()(this);
	}

	// Even if dead simulate entities
	memory::simulateEntities()(this);
//#endif
}

void Player_t::checkHasThinkFunction(bool isThinkingHint)
{
	memory::checkThinkFunction()(this, isThinkingHint);
}

void Player_t::restoreData(const char* context, int slot, int type)
{
	memory::restoreData()(this, context, slot, type);
}

void Player_t::saveData(const char* context, int slot, int type)
{
	memory::saveData()(this, context, slot, type);
}

bool Player_t::usingStandardWeaponsInVehicle()
{
	return memory::usingStandardWeaponsVehicle()(this);
}

bool Player_t::physicsRunThink(thinkmethods_t think)
{
	return memory::physicsRunThink()(this, think);
}

Weapon_t* Player_t::getActiveWeapon()
{
	return reinterpret_cast<Weapon_t*>(memory::interfaces::entList->getClientFromHandle(this->m_hActiveWeapon()));
}

Vec3 Player_t::getHitboxPos(const int id)
{
	if (auto modelStudio = memory::interfaces::modelInfo->getStudioModel(this->getModel()); modelStudio != nullptr)
	{
		if (auto hitbox = modelStudio->getHitboxSet(0)->getHitbox(id); hitbox != nullptr)
		{
			Vec3 min = math::transformVector(hitbox->m_bbmin, m_CachedBoneData().m_memory[hitbox->m_bone]);
			Vec3 max = math::transformVector(hitbox->m_bbmax, m_CachedBoneData().m_memory[hitbox->m_bone]);

			return Vec3{ min + max } *0.5f;
		}
	}

	return {};
}

Vec3 Player_t::getBonePos(const int id)
{
	return m_CachedBoneData().m_memory[id].origin();
}

Vec3 Player_t::getHitgroupPos(const int hitgroup)
{
	if (auto modelStudio = memory::interfaces::modelInfo->getStudioModel(this->getModel()); modelStudio != nullptr)
	{
		if (auto hitbox = modelStudio->getHitboxSet(this->m_nHitboxSet())->getHitbox(hitgroup); hitbox != nullptr)
		{
			Vec3 min = math::transformVector(hitbox->m_bbmin, m_CachedBoneData().m_memory[hitbox->m_bone]);
			Vec3 max = math::transformVector(hitbox->m_bbmax, m_CachedBoneData().m_memory[hitbox->m_bone]);
			return Vec3{ min + max } *0.5f;
		}
	}
	return {};
}

bool Player_t::isC4Owner()
{
	return memory::isC4Owner()(this);
}

std::string Player_t::getName()
{
	PlayerInfo_t info;
	memory::interfaces::engine->getPlayerInfo(this->getIndex(), &info);

	std::string name = info.m_name;

	if (name.length() > 20)
		name = name.substr(0, 20).append("...");

	// use when surface
	/*std::for_each(name.begin(), name.end(), [](char& el)
		{
			if (el < 0 || el > 128)
				el = '?';
		});*/

	return name;
}

std::string_view Player_t::getRawName()
{
	PlayerInfo_t info;
	memory::interfaces::engine->getPlayerInfo(this->getIndex(), &info);

	constexpr auto maxLength = 32U;

	std::array<char, maxLength> buf;
	const auto nameLength = std::min(maxLength - 1, strlen(info.m_name));
	std::memcpy(buf.data(), info.m_name, nameLength);
	buf.back() = '\0';

	return std::string_view{ buf };
}

int Player_t::getKills()
{
	return memory::interfaces::resourceInterface->getKills(this->getIndex());
}

int Player_t::getDeaths()
{
	return memory::interfaces::resourceInterface->getDeaths(this->getIndex());
}

int Player_t::getPing()
{
	return memory::interfaces::resourceInterface->getPing(this->getIndex());
}

std::string Player_t::getRank(bool useShortName)
{
	return memory::interfaces::resourceInterface->getRank(this->getIndex());
}

int Player_t::getWins()
{
	return memory::interfaces::resourceInterface->getWins(this->getIndex());
}

bool Player_t::isPossibleToSee(Player_t* player, const Vec3& pos)
{
	Trace_t tr{ };
	TraceFilter filter{ };
	filter.m_skip = this;
	memory::interfaces::trace->traceRay({ this->getEyePos(), pos }, MASK_PLAYER, &filter, &tr);

	return tr.m_entity == player || tr.m_fraction > 0.97f;
}

bool Player_t::isViewInSmoke(const Vec3& pos)
{
	return memory::throughSmoke()(this->getEyePos(), pos);
}

uintptr_t Player_t::getLiteralAddress()
{
	return *reinterpret_cast<uintptr_t*>(this);
}

CUtlVector<ClientHitVerify_t> Player_t::m_vecBulletVerifyListClient()
{
	const auto offset = memory::vecClientImpacts();
	return *reinterpret_cast<CUtlVector<ClientHitVerify_t>*>((uintptr_t)this + offset);
}

AABB_t Player_t::getOcclusionBounds()
{
	const auto col = this->collideable();
	const auto& mins = col->OBBMins();
	const auto& maxs = col->OBBMaxs();

	const auto m_usSolidFlags = col->getSolidFlags();
	const auto m_nSolidType = col->getSolid();

	auto isBoundsDefinedInEntitySpace = [=]()
	{
		return ((m_usSolidFlags & 0x40) == 0) &&
			(m_nSolidType != 2) && (m_nSolidType != 0);
	};

	if (!isBoundsDefinedInEntitySpace() || col->getCollisionAngles().isZero())
	{
		const auto& pos = this->absOrigin();
		return { pos + mins, pos + maxs };
	}
	else
	{
		Matrix3x4 mat = col->collisionToWorldTransform();
		auto [boundsMin, boundsMax] = math::transformAABB(mat, mins, maxs);
		return { boundsMin, boundsMax };
	}
}

#include "../ICvar.hpp"
#include "../ConVar.hpp"

AABB_t Player_t::getCameraBounds()
{
	const static auto occlusion_test_camera_margins = memory::interfaces::cvar->findVar("occlusion_test_camera_margins");
	const static auto occlusion_test_jump_margin = memory::interfaces::cvar->findVar("occlusion_test_jump_margin");

	const auto& pos = this->m_vecOrigin();
	const float cameraMargins = occlusion_test_camera_margins->getFloat();
	const float jumpMargin = occlusion_test_jump_margin->getFloat();

	return {
		pos + Vec3{ 0.0f, 0.0f, 46.0f } - Vec3{ cameraMargins, cameraMargins, 0.0f },
		pos + Vec3{ 0.0f, 0.0f, 64.0f } + Vec3{ cameraMargins, cameraMargins, jumpMargin }
	};
}

bool Player_t::isOtherTeam(Player_t* player)
{
	const static auto mp_teammates_are_enemies = memory::interfaces::cvar->findVar("mp_teammates_are_enemies");
	bool isDM = false;
	if (mp_teammates_are_enemies && mp_teammates_are_enemies->getInt())
		isDM = true;

	if (isDM && this->m_iTeamNum() == player->m_iTeamNum())
		return true;

	if (this->m_iTeamNum() != player->m_iTeamNum())
		return true;

	return false;
}

////////////////////////////////////////////////////////////////

Vec3 Inferno_t::getInfernoPos(size_t indexFire)
{
	return Vec3
	{
		static_cast<float>(m_fireXDelta()[indexFire]),
		static_cast<float>(m_fireYDelta()[indexFire]),
		static_cast<float>(m_fireZDelta()[indexFire])
	};
}