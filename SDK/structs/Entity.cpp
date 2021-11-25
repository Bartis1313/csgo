#include "Entity.hpp"
#undef max

////////////////////////////////////////////////////////////////////////

Vector Entity_t::getAimPunch()
{
	Vector vec = {};
	vfunc::callVFunc<void, PUNCH>(this, std::ref(vec));
	return vec;
}

matrix3x4_t& Entity_t::m_rgflCoordinateFrame()
{
	const static auto m_CollisionGroup = NetvarManager::g().getNetvar(XOR("DT_BaseEntity"), XOR("m_CollisionGroup"));
	// this was found in reclass, originally I had some bad resulting in CE addr
	const static auto validCoord = m_CollisionGroup - 0x30;

	return *reinterpret_cast<matrix3x4_t*>((uintptr_t)this + validCoord);
}

////////////////////////////////////////////////////////////////////////

std::string Weapon_t::getWpnName()
{
	switch (m_iItemDefinitionIndex())
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
	const auto idx = m_iItemDefinitionIndex();

	return idx == WEAPON_AK47 || idx == WEAPON_M4A1 || idx == WEAPON_M4A1_SILENCER || idx == WEAPON_GALILAR ||
		idx == WEAPON_FAMAS || idx == WEAPON_AUG || idx == WEAPON_SG553;
}

bool Weapon_t::isSmg()
{
	const auto idx = m_iItemDefinitionIndex();

	return idx == WEAPON_MAC10 || idx == WEAPON_MP7 || idx == WEAPON_MP9 || idx == WEAPON_P90 ||
		idx == WEAPON_BIZON || idx == WEAPON_UMP45 || idx == WEAPON_MP5SD;
}

bool Weapon_t::isShotgun()
{
	const auto idx = m_iItemDefinitionIndex();

	return idx == WEAPON_XM1014 || idx == WEAPON_NOVA || idx == WEAPON_SAWEDOFF || idx == WEAPON_MAG7 || idx == WEAPON_M249 || idx == WEAPON_NEGEV;
}

bool Weapon_t::isPistol()
{
	const auto idx = m_iItemDefinitionIndex();

	return idx == WEAPON_DEAGLE || idx == WEAPON_ELITE || idx == WEAPON_FIVESEVEN || idx == WEAPON_P250 ||
		idx == WEAPON_GLOCK || idx == WEAPON_HKP2000 || idx == WEAPON_CZ75A || idx == WEAPON_USP_SILENCER || idx == WEAPON_TEC9 || idx == WEAPON_REVOLVER;
}

bool Weapon_t::isSniper()
{
	const auto idx = m_iItemDefinitionIndex();

	return idx == WEAPON_AWP || idx == WEAPON_SCAR20 || idx == WEAPON_G3SG1 || idx == WEAPON_SSG08;
}

bool Weapon_t::isGrenade()
{
	const auto idx = m_iItemDefinitionIndex();

	return idx == WEAPON_FLASHBANG || idx == WEAPON_HEGRENADE || idx == WEAPON_SMOKEGRENADE || idx == WEAPON_MOLOTOV || idx == WEAPON_DECOY || idx == WEAPON_INCGRENADE;
}

bool Weapon_t::isKnife()
{
	const auto idx = m_iItemDefinitionIndex();

	return idx == WEAPON_KNIFE || idx == WEAPON_KNIFE_BAYONET || idx == WEAPON_KNIFE_BUTTERFLY || idx == WEAPON_KNIFE_FALCHION
		|| idx == WEAPON_KNIFE_FLIP || idx == WEAPON_KNIFE_GUT || idx == WEAPON_KNIFE_KARAMBIT || idx == WEAPON_KNIFE_M9_BAYONET
		|| idx == WEAPON_KNIFE_PUSH || idx == WEAPON_KNIFE_SURVIVAL_BOWIE || idx == WEAPON_KNIFE_T || idx == WEAPON_KNIFE_TACTICAL
		|| idx == WEAPON_KNIFEGG || idx == WEAPON_KNIFE_GHOST || idx == WEAPON_KNIFE_GYPSY_JACKKNIFE || idx == WEAPON_KNIFE_STILETTO
		|| idx == WEAPON_KNIFE_URSUS || idx == WEAPON_KNIFE_WIDOWMAKER || idx == WEAPON_KNIFE_CSS || idx == WEAPON_KNIFE_CANIS
		|| idx == WEAPON_KNIFE_CORD || idx == WEAPON_KNIFE_OUTDOOR || idx == WEAPON_KNIFE_SKELETON;
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
	return info.name;
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
	Ray_t ray;
	TraceFilter filter;

	auto start = this->getEyePos();
	auto end = pos;
	filter.m_skip = this;
	ray.initialize(start, pos);
	interfaces::trace->traceRay(ray, MASK_VISIBLE, &filter, &tr);

	return tr.m_entity == ent || tr.m_fraction > 1.0f;
}

////////////////////////////////////////////////////////////////
