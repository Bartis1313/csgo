#include "Entity.hpp"
#include <array>

#include "../IVModelInfo.hpp"
#include "../ClientClass.hpp"
#include "../Enums.hpp"
#include "../ICollideable.hpp"
#include "../IClientEntityList.hpp"
#include "../math/matrix.hpp"
#include "../math/Vector.hpp"
#include "../../utilities/math/math.hpp"
#include "../IVEngineClient.hpp"
#include "../CPlayerResource.hpp"
#include "../IEngineTrace.hpp"
#include "../IWeapon.hpp"

////////////////////////////////////////////////////////////////////////

Vector Entity_t::getAimPunch()
{
	Vector vec = {};
	vfunc::callVFunc<void, PUNCH>(this, std::ref(vec));
	return vec;
}

AnimationLayer* Entity_t::getAnimOverlays()
{
	const static auto offset = *reinterpret_cast<uintptr_t*>(utilities::patternScan(CLIENT_DLL, ANIMATION_LAYER, 0x2)); // 0x2990
	return *reinterpret_cast<AnimationLayer**>(uintptr_t(this) + offset);
}

size_t Entity_t::getSequenceActivity(size_t sequence)
{
	auto studio = interfaces::modelInfo->getStudioModel(this->getModel());
	if (!studio)
		return 0;

	using fn = int(__fastcall*)(void*, void*, int);
	const static auto originalGSA = reinterpret_cast<fn>(utilities::patternScan(CLIENT_DLL, SEQUENCE_ACTIVITY));
	return originalGSA(this, studio, sequence);
}

bool Entity_t::isBreakable()
{
	if (!this)
		return false;

	if (!this->getIndex())
		return false;

	using fn = bool(__thiscall*)(void*);
	const static auto isBreakablefn = reinterpret_cast<fn>(utilities::patternScan(CLIENT_DLL, IS_BREAKBLE));

	if (bool res = isBreakablefn(this); !res)
		return false;

	auto cl = this->clientClass();
	if (!cl)
		return false;

	auto id = cl->m_classID;

	constexpr std::array breakableIds = // for surface ids ONLY!
	{
		CBaseDoor,
		CBreakableSurface,
	};

	if (auto it = std::find(std::cbegin(breakableIds), std::cend(breakableIds), id); it != breakableIds.cend()) // if id was found from surface
		return true;

	// there we finally check actual entity
	if (id = CBaseEntity && this->collideable()->getSolid() == 1) // can mess with masks, but it's the same
		return true;

	return false;
}

////////////////////////////////////////////////////////////////////////

std::string Weapon_t::getWpnName()
{
	switch (this->m_iItemDefinitionIndex())
	{
	case WEAPON_KNIFE: return "KNIFE";
	case WEAPON_KNIFE_T: return "KNIFE";
	case WEAPON_KNIFE_BAYONET: return "KNIFE";
	case WEAPON_KNIFE_SURVIVAL_BOWIE: return "KNIFE";
	case WEAPON_KNIFE_BUTTERFLY: return "KNIFE";
	case WEAPON_KNIFE_FALCHION: return "KNIFE";
	case WEAPON_KNIFE_FLIP: return "KNIFE";
	case WEAPON_KNIFE_GUT: return "KNIFE";
	case WEAPON_KNIFE_KARAMBIT: return "KNIFE";
	case WEAPON_KNIFE_M9_BAYONET: return "KNIFE";
	case WEAPON_KNIFE_TACTICAL: return "KNIFE";
	case WEAPON_KNIFE_PUSH: return "KNIFE";
	case WEAPON_DEAGLE: return "DEAGLE";
	case WEAPON_ELITE: return "DUAL BERETTAS";
	case WEAPON_FIVESEVEN:return "FIVE-SEVEN";
	case WEAPON_GLOCK: return "GLOCK 18";
	case WEAPON_HKP2000: return " P2000";
	case WEAPON_P250: return "P250";
	case WEAPON_USP_SILENCER: return "USP-S";
	case WEAPON_TEC9: return "TEC-9";
	case WEAPON_REVOLVER: return "REVOLVER";
	case WEAPON_MAC10: return "MAC-10";
	case WEAPON_UMP45: return "UMP-45";
	case WEAPON_BIZON: return "PP-BIZON";
	case WEAPON_MP7: return "MP7";
	case WEAPON_MP9: return "MP9";
	case WEAPON_P90: return "P90";
	case WEAPON_GALILAR: return "GALIL AR";
	case WEAPON_FAMAS: return "FAMAS";
	case WEAPON_M4A1_SILENCER: return "M4A1-S";
	case WEAPON_M4A1: return "M4A4";
	case WEAPON_AUG: return "AUG";
	case WEAPON_SG553: return "SG 553";
	case WEAPON_AK47: return "AK-47";
	case WEAPON_G3SG1: return "G3SG1";
	case WEAPON_SCAR20: return "SCAR-20";
	case WEAPON_AWP: return "AWP";
	case WEAPON_SSG08: return "SSG 08";
	case WEAPON_XM1014: return "XM1014";
	case WEAPON_SAWEDOFF: return "SAWED-OFF";
	case WEAPON_MAG7: return "MAG-7";
	case WEAPON_NOVA: return "NOVA";
	case WEAPON_NEGEV: return "NEGEV";
	case WEAPON_M249: return "M249";
	case WEAPON_TASER: return "ZEUS X27";
	case WEAPON_FLASHBANG: return "FLASHBANG";
	case WEAPON_HEGRENADE: return "HE GRENADE";
	case WEAPON_SMOKEGRENADE: return "SMOKE";
	case WEAPON_MOLOTOV: return "MOLOTOV";
	case WEAPON_DECOY: return "DECOY";
	case WEAPON_INCGRENADE: return "INCENDIARY";
	case WEAPON_C4: return "C4";
	case WEAPON_CZ75A:return "CZ75-AUTO";
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

void Player_t::setAbsOrigin(const Vector& origin)
{
	using fn = void(__thiscall*)(void*, const Vector&);
	const static auto setabs = reinterpret_cast<fn>(utilities::patternScan(CLIENT_DLL, SETABSORIGIN));
	setabs(this, std::cref(origin));
}

Weapon_t* Player_t::getActiveWeapon()
{
	return reinterpret_cast<Weapon_t*>(interfaces::entList->getClientFromHandle(this->m_hActiveWeapon()));
}

Vector Player_t::getHitboxPos(const int id)
{
	if (Matrix3x4 matBone[BONE_USED_BY_HITBOX]; setupBones(matBone, BONE_USED_BY_HITBOX, BONE_USED_BY_HITBOX, 0.0f))
	{
		if (auto modelStudio = interfaces::modelInfo->getStudioModel(this->getModel()); modelStudio != nullptr)
		{
			if (auto hitbox = modelStudio->getHitboxSet(0)->getHitbox(id); hitbox != nullptr)
			{
				Vector min = math::transformVector(hitbox->m_bbmin, matBone[hitbox->m_bone]);
				Vector max = math::transformVector(hitbox->m_bbmax, matBone[hitbox->m_bone]);

				return Vector((min.x + max.x) * 0.5f, (min.y + max.y) * 0.5f, (min.z + max.z) * 0.5f);
			}
		}
	}

	return {};
}

Vector Player_t::getBonePos(const int id)
{
	if (Matrix3x4 matBone[BONE_USED_BY_HITBOX]; setupBones(matBone, BONE_USED_BY_HITBOX, BONE_USED_BY_HITBOX, 0.0f))
		return matBone[id].origin();
	
	return {};
}

bool Player_t::isC4Owner()
{
	using fn = bool(__thiscall*)(void*);
	const static auto isc4 = reinterpret_cast<fn>(utilities::patternScan(CLIENT_DLL, HASC4));
	return isc4(this);
}

std::string Player_t::getName()
{
	PlayerInfo_t info;
	interfaces::engine->getPlayerInfo(this->getIndex(), &info);

	std::string name = info.m_name;

	if (name.length() > 20)
		name = name.substr(0, 20).append(XOR("..."));

	// use when surface
	/*std::for_each(name.begin(), name.end(), [](char& el)
		{
			if (el < 0 || el > 128)
				el = '?';
		});*/

	return name;
}

int Player_t::getKills()
{
	const static auto res = *interfaces::resource;
	if (res)
		return res->getKills(this->getIndex());

	return 0;
}

int Player_t::getDeaths()
{
	const static auto res = *interfaces::resource;
	if (res)
		return res->getDeaths(this->getIndex());

	return 0;
}

int Player_t::getPing()
{
	const static auto res = *interfaces::resource;
	if (res)
		return res->getPing(this->getIndex());

	return 0;
}

bool Player_t::isPossibleToSee(const Vector& pos)
{
	Trace_t tr;
	TraceFilter filter;
	filter.m_skip = this;
	interfaces::trace->traceRay({ this->getEyePos(), pos }, MASK_PLAYER, &filter, &tr);

	return tr.m_entity == this || tr.m_fraction > 0.97f;
}

uintptr_t Player_t::getLiteralAddress()
{
	return *reinterpret_cast<uintptr_t*>(this);
}

////////////////////////////////////////////////////////////////

Vector Inferno_t::getInfernoPos(size_t indexFire)
{
	return Vector{
		static_cast<float>(m_fireXDelta()[indexFire]),
		static_cast<float>(m_fireYDelta()[indexFire]),
		static_cast<float>(m_fireZDelta()[indexFire]) };
}
