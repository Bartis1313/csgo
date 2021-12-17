#include "Entity.hpp"
#undef max
#include <format>

////////////////////////////////////////////////////////////////////////

Vector Entity_t::getAimPunch()
{
	Vector vec = {};
	vfunc::callVFunc<void, PUNCH>(this, std::ref(vec));
	return vec;
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
	return this->getWpnInfo()->m_type == WEAPONTYPE_RIFLE;
}

bool Weapon_t::isSmg()
{
	return this->getWpnInfo()->m_type == WEAPONTYPE_MACHINEGUN;
}

bool Weapon_t::isShotgun()
{
	return this->getWpnInfo()->m_type == WEAPONTYPE_SHOTGUN;
}

bool Weapon_t::isPistol()
{
	return this->getWpnInfo()->m_type == WEAPONTYPE_PISTOL;
}

bool Weapon_t::isSniper()
{
	return this->getWpnInfo()->m_type == WEAPONTYPE_SNIPER_RIFLE;
}

bool Weapon_t::isGrenade()
{
	return this->getWpnInfo()->m_type == WEAPONTYPE_GRENADE;
}

bool Weapon_t::isKnife()
{
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
	if (matrix3x4_t matBone[BONE_USED_BY_HITBOX]; setupBones(matBone, BONE_USED_BY_HITBOX, BONE_USED_BY_HITBOX, 0.0f))
	{
		if (auto modelStudio = interfaces::modelInfo->getStudioModel(this->getModel()); modelStudio != nullptr)
		{
			if (auto hitbox = modelStudio->getHitboxSet(0)->getHitbox(id); hitbox != nullptr)
			{
				auto min = Vector{}, max = Vector{};

				math::transformVector(hitbox->m_bbmin, matBone[hitbox->m_bone], min);
				math::transformVector(hitbox->m_bbmax, matBone[hitbox->m_bone], max);

				return Vector((min.x + max.x) * 0.5f, (min.y + max.y) * 0.5f, (min.z + max.z) * 0.5f);
			}
		}
	}

	return NOTHING;
}

Vector Player_t::getBonePosition(const int id)
{
	if (matrix3x4_t matBone[BONE_USED_BY_HITBOX]; setupBones(matBone, BONE_USED_BY_HITBOX, BONE_USED_BY_HITBOX, 0.0f))
		return matBone[id].origin();
	
	return NOTHING;
}

bool Player_t::isC4Owner()
{
	using fn = bool(__thiscall*)(void*);
	const static auto isc4 = reinterpret_cast<fn>(utilities::patternScan(CLIENT_DLL, HASC4));
	return isc4(this);
}

std::string Player_t::getName()
{
	playerInfo_t info;
	interfaces::engine->getPlayerInfo(this->getIndex(), &info);

	std::string name = info.name;

	if (name.length() > 20)
		name = name.substr(0, 20).append(XOR("..."));

	std::for_each(name.begin(), name.end(), [](char& el)
		{
			if (el < 0 || el > 128)
				el = '?';
		});

	return name;
}

int Player_t::getKills()
{
	const static auto res = *interfaces::resource;
	if (res)
	{
		const static auto kills = res->getKills(this->getIndex());
		return kills;
	}
	return 0;
}

int Player_t::getDeaths()
{
	const static auto res = *interfaces::resource;
	if (res)
	{
		const static auto deaths = res->getDeaths(this->getIndex());
		return deaths;
	}
	return 0;
}

int Player_t::getPing()
{
	const static auto res = *interfaces::resource;
	if (res)
	{
		const static auto ping = res->getPing(this->getIndex());
		return ping;
	}
	return 0;
}

bool Player_t::isPossibleToSee(Player_t* ent, const Vector& pos)
{
	Trace_t tr;
	TraceFilter filter;

	auto start = this->getEyePos();
	auto end = pos;
	filter.m_skip = this;
	interfaces::trace->traceRay({ this->getEyePos(), pos }, MASK_PLAYER, &filter, &tr);

	return tr.m_entity == this || tr.m_fraction > 0.97f;

}

////////////////////////////////////////////////////////////////
