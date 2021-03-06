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
	const static auto offset = *reinterpret_cast<uintptr_t*>(utilities::patternScan(CLIENT_DLL, CACHED_BONE, 0x2)) + 0x4; // 0x2914
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

std::u8string Weapon_t::getIcon(int correctIndex)
{
	// 0xE000 - start = WEAPON_NONE

	switch (auto idx = correctIndex != -1 ? correctIndex : this->m_iItemDefinitionIndex(); idx)
	{
	case WEAPON_DEAGLE:					return u8"\uE001";
	case WEAPON_ELITE:					return u8"\uE002";
	case WEAPON_FIVESEVEN:				return u8"\uE003";
	case WEAPON_GLOCK:					return u8"\uE004";
	case WEAPON_AK47:					return u8"\uE007";
	case WEAPON_AUG:					return u8"\uE008";
	case WEAPON_AWP:					return u8"\uE009";
	case WEAPON_FAMAS:					return u8"\uE00A";
	case WEAPON_G3SG1:					return u8"\uE00B";
	case WEAPON_GALILAR:				return u8"\uE00D";
	case WEAPON_M249:					return u8"\uE00E";
	case WEAPON_M4A1:					return u8"\uE010";
	case WEAPON_MAC10:					return u8"\uE011";
	case WEAPON_P90:					return u8"\uE013";
	case WEAPON_ZONE_REPULSOR:			return u8"\uE014";
	case WEAPON_MP5SD:					return u8"\uE017";
	case WEAPON_UMP45:					return u8"\uE018";
	case WEAPON_XM1014:					return u8"\uE019";
	case WEAPON_BIZON:					return u8"\uE01A";
	case WEAPON_MAG7:					return u8"\uE01B";
	case WEAPON_NEGEV:					return u8"\uE01C";
	case WEAPON_SAWEDOFF:				return u8"\uE01D";
	case WEAPON_TEC9:					return u8"\uE01E";
	case WEAPON_TASER:					return u8"\uE01F";
	case WEAPON_HKP2000:				return u8"\uE020";
	case WEAPON_MP7:					return u8"\uE021";
	case WEAPON_MP9:					return u8"\uE022";
	case WEAPON_NOVA:					return u8"\uE023";
	case WEAPON_P250:					return u8"\uE024";
	case WEAPON_SHIELD:					return u8"\uE025";
	case WEAPON_SCAR20:					return u8"\uE026";
	case WEAPON_SG553:					return u8"\uE027";
	case WEAPON_SSG08:					return u8"\uE028";
	case WEAPON_KNIFEGG:				return u8"\uE029";
	case WEAPON_KNIFE:					return u8"\uE02A";
	case WEAPON_FLASHBANG:				return u8"\uE02B";
	case WEAPON_HEGRENADE:				return u8"\uE02C";
	case WEAPON_SMOKEGRENADE:			return u8"\uE02D";
	case WEAPON_MOLOTOV:				return u8"\uE02E";
	case WEAPON_DECOY:					return u8"\uE02F";
	case WEAPON_INCGRENADE:				return u8"\uE030";
	case WEAPON_C4:						return u8"\uE031";
	case WEAPON_HEALTHSHOT:				return u8"\uE039";
	case WEAPON_KNIFE_T:				return u8"\uE03B";
	case WEAPON_M4A1_SILENCER:			return u8"\uE03C";
	case WEAPON_USP_SILENCER:			return u8"\uE03D";
	case WEAPON_CZ75A:					return u8"\uE03F";
	case WEAPON_REVOLVER:				return u8"\uE040";
	case WEAPON_TAGRENADE:				return u8"\uE044";
	case WEAPON_FISTS:					return u8"\uE045";
	case WEAPON_BREACHCHARGE:			return u8"\uE046";
	case WEAPON_TABLET:					return u8"\uE048";
	case WEAPON_MELEE:					return u8"\uE04A";
	case WEAPON_AXE:					return u8"\uE04B";
	case WEAPON_HAMMER:					return u8"\uE04C";
	case WEAPON_SPANNER:				return u8"\uE04E";
	case WEAPON_KNIFE_GHOST:			return u8"\uE050";
	case WEAPON_FIREBOMB:				return u8"\uE051";
	case WEAPON_DIVERSION:				return u8"\uE052";
	case WEAPON_FRAG_GRENADE:			return u8"\uE053";
	case WEAPON_SNOWBALL:				return u8"\uE054";
	case WEAPON_BUMPMINE:				return u8"\uE055";
	case WEAPON_KNIFE_BAYONET:			return u8"\uE1F4";
	case WEAPON_KNIFE_CSS:				return u8"\uE1F7";
	case WEAPON_KNIFE_FLIP:				return u8"\uE1F9";
	case WEAPON_KNIFE_GUT:				return u8"\uE1FA";
	case WEAPON_KNIFE_KARAMBIT:			return u8"\uE1FB";
	case WEAPON_KNIFE_M9_BAYONET:		return u8"\uE1FC";
	case WEAPON_KNIFE_TACTICAL:			return u8"\uE1FD";
	case WEAPON_KNIFE_FALCHION:			return u8"\uE200";
	case WEAPON_KNIFE_SURVIVAL_BOWIE:	return u8"\uE202";
	case WEAPON_KNIFE_BUTTERFLY:		return u8"\uE203";
	case WEAPON_KNIFE_PUSH:				return u8"\uE204";
	case WEAPON_KNIFE_CORD:				return u8"\uE205";
	case WEAPON_KNIFE_CANIS:			return u8"\uE206";
	case WEAPON_KNIFE_URSUS:			return u8"\uE207";
	case WEAPON_KNIFE_GYPSY_JACKKNIFE:	return u8"\uE208";
	case WEAPON_KNIFE_OUTDOOR:			return u8"\uE209";
	case WEAPON_KNIFE_STILETTO:			return u8"\uE20A";
	case WEAPON_KNIFE_WIDOWMAKER:		return u8"\uE20B";
	case WEAPON_KNIFE_SKELETON:			return u8"\uE20D";
	default:
		return u8"\u0000";
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
	/*if (Matrix3x4 matBone[MAX_BONES]; setupBones(matBone, MAX_BONES, BONE_USED_BY_HITBOX, 0.0f))
	{
		if (auto modelStudio = interfaces::modelInfo->getStudioModel(this->getModel()); modelStudio != nullptr)
		{
			if (auto hitbox = modelStudio->getHitboxSet(0)->getHitbox(id); hitbox != nullptr)
			{
				Vector min = math::transformVector(hitbox->m_bbmin, matBone[hitbox->m_bone]);
				Vector max = math::transformVector(hitbox->m_bbmax, matBone[hitbox->m_bone]);

				return Vector{ min + max } * 0.5f;
			}
		}
	}*/

	if (auto modelStudio = interfaces::modelInfo->getStudioModel(this->getModel()); modelStudio != nullptr)
	{
		if (auto hitbox = modelStudio->getHitboxSet(0)->getHitbox(id); hitbox != nullptr)
		{
			Vector min = math::transformVector(hitbox->m_bbmin, m_CachedBoneData().m_memory[hitbox->m_bone]);
			Vector max = math::transformVector(hitbox->m_bbmax, m_CachedBoneData().m_memory[hitbox->m_bone]);

			return Vector{ min + max } *0.5f;
		}
	}

	return {};
}

Vector Player_t::getBonePos(const int id)
{
	return m_CachedBoneData().m_memory[id].origin();
}

Vector Player_t::getHitgroupPos(const int hitgroup)
{
	auto fixHitgroupIndex = [h = hitgroup]()
	{
		switch (h)
		{
		case HITGROUP_HEAD:
			return HITBOX_HEAD;
		case HITGROUP_CHEST:
			return HITBOX_LOWER_CHEST;
		case HITGROUP_STOMACH:
			return HITBOX_BELLY;
		case HITGROUP_LEFTARM:
			return HITBOX_RIGHT_HAND;
		case HITGROUP_RIGHTARM:
			return HITBOX_LEFT_HAND;
		case HITGROUP_LEFTLEG:
			return HITBOX_RIGHT_CALF;
		case HITGROUP_RIGHTLEG:
			return HITBOX_LEFT_CALF;
		default:
			return HITBOX_PELVIS;
		}
	};

	if (auto modelStudio = interfaces::modelInfo->getStudioModel(this->getModel()); modelStudio != nullptr)
	{
		if (auto hitbox = modelStudio->getHitboxSet(this->m_nHitboxSet())->getHitbox(fixHitgroupIndex()); hitbox != nullptr)
		{
			Vector min = math::transformVector(hitbox->m_bbmin, m_CachedBoneData().m_memory[hitbox->m_bone]);
			Vector max = math::transformVector(hitbox->m_bbmax, m_CachedBoneData().m_memory[hitbox->m_bone]);
			return Vector{ min + max } *0.5f;
		}
	}
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

	return -1;
}

int Player_t::getDeaths()
{
	const static auto res = *interfaces::resource;
	if (res)
		return res->getDeaths(this->getIndex());

	return -1;
}

int Player_t::getPing()
{
	const static auto res = *interfaces::resource;
	if (res)
		return res->getPing(this->getIndex());

	return -1;
}

std::string Player_t::getRank(bool useShortName)
{
	const static auto res = *interfaces::resource;
	if (res)
		return res->getRank(this->getIndex(), useShortName);

	return "unk";
}

int Player_t::getWins()
{
	const static auto res = *interfaces::resource;
	if (res)
		return res->getWins(this->getIndex());

	return -1;
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
