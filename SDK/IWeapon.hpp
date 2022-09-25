#pragma once

#include <utilities/pad.hpp>
#include <utilities/vfunc.hpp>

class WeaponInfo
{
public:
	PAD(20);
	int m_maxClip1;
	int m_maxClip2;
	PAD(8);
	int m_primaryMaxAmmo;
	int m_secondaryMaxAmmo;
	PAD(92);
	const char* m_WeaponName;
	PAD(60);
	int m_type;
	PAD(4);
	int m_price;
	int m_priceKKillAward;
	PAD(20);
	bool isFullAuto;
	PAD(3);
	int m_damage;
	float m_hsMultiplier;
	float m_armorRatio;
	int m_bullets;
	float m_penetration;
	PAD(8);
	float m_range;
	float m_rangeModyfier;
	float m_throwVelocity;
	PAD(12);
	bool isSilenced;
	PAD(123);
	//float m_recoil; not sure
};

class IWeapon
{
public:
	VFUNC(WeaponInfo*, getWpnData, 2, (unsigned int ItemDefinitionIndex), (this, ItemDefinitionIndex));
};