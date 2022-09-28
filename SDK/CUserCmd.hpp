#pragma once

#include "math/Vector.hpp"
#include "checksum_crc.hpp"

#include <utilities/pad.hpp>

class CUserCmd
{
public:
	CRC32_t getChecksum(void) const
	{
		CRC32_t crc;

		CRC32_Init(&crc);
		CRC32_ProcessBuffer(&crc, &m_commandNumber, sizeof(m_commandNumber));
		CRC32_ProcessBuffer(&crc, &m_tickcount, sizeof(m_tickcount));
		CRC32_ProcessBuffer(&crc, &m_viewangles, sizeof(m_viewangles));
		CRC32_ProcessBuffer(&crc, &m_aimdirection, sizeof(m_aimdirection));
		CRC32_ProcessBuffer(&crc, &m_forwardmove, sizeof(m_forwardmove));
		CRC32_ProcessBuffer(&crc, &m_sidemove, sizeof(m_sidemove));
		CRC32_ProcessBuffer(&crc, &m_upmove, sizeof(m_upmove));
		CRC32_ProcessBuffer(&crc, &m_buttons, sizeof(m_buttons));
		CRC32_ProcessBuffer(&crc, &m_impulse, sizeof(m_impulse));
		CRC32_ProcessBuffer(&crc, &m_weaponSelect, sizeof(m_weaponSelect));
		CRC32_ProcessBuffer(&crc, &m_weaponSubtype, sizeof(m_weaponSubtype));
		CRC32_ProcessBuffer(&crc, &m_randomSeed, sizeof(m_randomSeed));
		CRC32_ProcessBuffer(&crc, &m_mousedx, sizeof(m_mousedx));
		CRC32_ProcessBuffer(&crc, &m_mousedy, sizeof(m_mousedy));
		CRC32_Final(&crc);

		return crc;
	}

	PAD(4);
	int m_commandNumber;
	int m_tickcount;
	Vec3 m_viewangles;
	Vec3 m_aimdirection;
	float m_forwardmove;
	float m_sidemove;
	float m_upmove;
	int m_buttons;
	char m_impulse;
	int m_weaponSelect;
	int m_weaponSubtype;
	int m_randomSeed;
	short m_mousedx;
	short m_mousedy;
	bool m_predicted;
	PAD(24);
};

class CVerifiedUserCmd
{
public:
	CUserCmd m_cmd;
	unsigned long m_crc;
};