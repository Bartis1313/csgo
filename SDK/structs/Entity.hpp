#pragma once

#include "../../utilities/netvars/netvars.hpp"
#include "../interfaces/interfaces.hpp"
#include "IDXandPaterrns.hpp"
#include "../../utilities/math/math.hpp"

class Weapon_t;
class Player_t;

// Virtual functions
// 0x4 - renderable
// 0x8 - networkable

/*
* This file is so far probably the easiest when it comes to netvar usage
* You find the netvar - throw table and name, pretty easy
* in my source I accept only "raw" names for netvars, it's more reliable for me
* Sometimes values need extra adress or are pointers
* Functions that return something as value from some value on the stack in function
* example:
* void blabla() { auto i = 10; return i;} -> i is on the stack, those functions I keep in cpp
* 
* Updated 01:25 05.11.2021
* There are many missing, with time and those which I need, I will add
*/


////////////////////////////////////////////////////////////////////////////////////////////

class Entity_t
{
public:
	NETVAR(int, m_iTeamNum, "DT_CSPlayer", "m_iTeamNum");
	NETVAR(Vector, m_vecOrigin, "DT_BasePlayer", "m_vecOrigin");
	NETVAR(int, m_hOwnerEntity, "DT_BaseEntity", "m_hOwnerEntity");
	NETVAR(int, m_CollisionGroup, "DT_BasePlayer", "m_CollisionGroup");
	NETVAR(Vector, m_ViewOffset, "DT_BasePlayer", "m_vecViewOffset[0]");
	NETVAR(bool, m_bSpotted, "DT_BaseEntity", "m_bSpotted");
	NETVAR(float, m_flSimulationTime, "DT_BasePlayer", "m_flSimulationTime");
	NETVAR_ADDR(int, moveType, "DT_BasePlayer", "m_nRenderMode", 1);

	VFUNC(Vector&, absOrigin, 10, (), (this));
	VFUNC(Vector&, absAngles, 11, (), (this));
	VFUNC(ClientClass*, clientClass, 2, (), (this + 0x8));
	VFUNC(ICollideable*, colliedable, 3, (), (this));
	VFUNC(int, getIndex, 10, (), (this + 0x8));
	VFUNC(bool, isPlayer, ISPLAYER, (), (this));
	VFUNC(bool, isWeapon, ISWEAPON, (), (this));
	VFUNC(bool, setupBones, 13, (matrix3x4_t* out, int maxBones, int mask, float time), (this + 0x4, out, maxBones, mask, time));
	VFUNC(model_t*, getModel, 8, (), (this + 0x4));
	VFUNC(int, drawModel, 9, (int flags, uint8_t alpha), (this + 0x4, flags, alpha));
	VFUNC(bool, isDormant, 9, (), (this + 0x8));

	[[nodiscard]] Vector getAimPunch();
	[[nodiscard]] matrix3x4_t& m_rgflCoordinateFrame();
};

////////////////////////////////////////////////////////////////////////////////////////////

class Bomb_t : public Entity_t
{
public:
	NETVAR(float, m_flDefuseCountDown, "DT_PlantedC4", "m_flDefuseCountDown");
	NETVAR(int, m_hBombDefuser, "DT_PlantedC4", "m_hBombDefuser");
	NETVAR(float, m_flC4Blow, "DT_PlantedC4", "m_flC4Blow");
	NETVAR(bool, m_bBombDefused, "DT_PlantedC4", "m_bBombDefused");
};

////////////////////////////////////////////////////////////////////////////////////////////

class Weapon_t : Entity_t
{
public:
	NETVAR(float, m_flNextPrimaryAttack, "DT_BaseCombatWeapon", "m_flNextPrimaryAttack");
	NETVAR(float, m_flNextSecondaryAttack, "DT_BaseCombatWeapon", "m_flNextSecondaryAttack");
	NETVAR(short, m_iItemDefinitionIndex, "DT_BaseCombatWeapon", "m_iItemDefinitionIndex");
	NETVAR(int, m_iClip1, "DT_BaseCombatWeapon", "m_iClip1");
	NETVAR(int, m_iClip2, "DT_BaseCombatWeapon", "m_iClip2");
	NETVAR(int, m_iPrimaryReserveAmmoCount, "DT_BaseCombatWeapon", "m_iPrimaryReserveAmmoCount");
	NETVAR(int, m_iEntityQuality, "DT_BaseCombatWeapon", "m_iEntityQuality");
	NETVAR(bool, m_bBurstMode, "DT_BaseCombatWeapon", "m_bBurstMode");
	NETVAR(int, m_iBurstShotsRemaining, "DT_BaseCombatWeapon", "m_iBurstShotsRemaining");
	NETVAR(int, m_zoomLevel, "DT_WeaponCSBaseGun", "m_zoomLevel");

	VFUNC(float, getInaccuracy, INACCURACY, (), (this));
	VFUNC(float, getSpread, SPREAD, (), (this));
	VFUNC(WeaponInfo*, getWpnInfo, WEAPONINFO, (), (this))

	[[nodiscard]]  std::string getWpnName();

	[[nodiscard]] bool isEmpty() { return m_iClip1() <= 0; }
	[[nodiscard]] bool isRifle();
	[[nodiscard]] bool isSmg();
	[[nodiscard]] bool isShotgun();
	[[nodiscard]] bool isPistol();
	[[nodiscard]] bool isSniper();
	[[nodiscard]] bool isGrenade();
	[[nodiscard]] bool isKnife();
	[[nodiscard]] bool isNonAimable();
};

////////////////////////////////////////////////////////////////////////////////////////////

class Player_t : public Entity_t
{
public:
	// added many here, it's late night as I am writing it, I think those will be enough for all time this cheat is developed

	NETVAR(Vector, m_angEyeAngles, "DT_CSPlayer", "m_angEyeAngles");
	NETVAR(Vector, m_viewPunchAngle, "DT_BasePlayer", "m_viewPunchAngle");
	NETVAR(Vector, m_aimPunchAngle, "DT_BasePlayer", "m_aimPunchAngle");
	NETVAR(Vector, m_vecVelocity, "DT_BasePlayer", "m_vecVelocity[0]");
	NETVAR(Vector, m_vecViewOffset, "DT_BasePlayer", "m_vecViewOffset[0]");
	NETVAR(int, m_iHealth, "DT_CSPlayer", "m_iHealth");
	NETVAR(int, m_ArmorValue, "DT_CSPlayer", "m_ArmorValue");
	NETVAR(bool, m_bIsScoped, "DT_CSPlayer", "m_bIsScoped");
	NETVAR(bool, m_bHasHelmet, "DT_CSPlayer", "m_bHasHelmet");
	NETVAR(int, m_iShotsFired, "DT_CSPlayer", "m_iShotsFired");
	NETVAR(bool, m_bGunGameImmunity, "DT_CSPlayer", "m_bGunGameImmunity");
	NETVAR(float, m_flNextAttack, "DT_CSPlayer", "m_flNextAttack");
	NETVAR(bool, m_bHasDefuser, "DT_CSPlayer", "m_bHasDefuser");
	NETVAR(int, m_hViewModel, "DT_BasePlayer", "m_hViewModel[0]");
	NETVAR(float, m_flLowerBodyYawTarget, "DT_CSPlayer", "m_flLowerBodyYawTarget");
	NETVAR(float, m_flFlashDuration, "DT_CSPlayer", "m_flFlashDuration");
	NETVAR(int, m_lifeState, "DT_CSPlayer", "m_lifeState");
	NETVAR(int, m_fFlags, "DT_CSPlayer", "m_fFlags");
	NETVAR(int, m_nHitboxSet, "DT_CSPlayer", "m_nHitboxSet");
	NETVAR(int, m_nTickBase, "DT_CSPlayer", "m_nTickBase");
	NETVAR(float, m_flDuckSpeed, "DT_CSPlayer", "m_flDuckSpeed");
	NETVAR(float, m_flDuckAmount, "DT_CSPlayer", "m_flDuckAmount");
	NETVAR(bool, m_bDucked, "DT_CSPlayer", "m_bDucked");
	NETVAR(bool, m_bDucking, "DT_CSPlayer", "m_bDucking");
	NETVAR(float, m_flHealthShotBoostExpirationTime, "DT_CSPlayer", "m_flHealthShotBoostExpirationTime");
	NETVAR(unsigned long, m_hObserverTarget, "DT_BasePlayer", "m_hObserverTarget");
	NETVAR(int, m_hActiveWeapon,"DT_CSPlayer", "m_hActiveWeapon");

	void setAbsOrigin(const Vector& origin);
	[[nodiscard]] Weapon_t* getActiveWeapon();
	[[nodiscard]] Vector getEyePos() { return m_vecOrigin() + m_ViewOffset(); } // there is some vtable iirc, but this is safer when it comes with updates
	[[nodiscard]] bool isAlive() { return m_iHealth() > 0; }
	[[nodiscard]] bool isInAir() { return !(m_fFlags() & FL_ONGROUND); }
	[[nodiscard]] bool IsValid() { return (isAlive() && !isDormant()); }

	[[nodiscard]] Vector getHitboxPos(int id);
	[[nodiscard]] Vector getBonePosition(int id);
	[[nodiscard]] bool isC4Owner();
	[[nodiscard]] std::string getName();
	[[nodiscard]] int getKills(int id);
	[[nodiscard]] int getDeaths(int id);
	[[nodiscard]] int getPing(int id);
};

////////////////////////////////////////////////////////////////////////////////////////////