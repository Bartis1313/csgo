#pragma once
#include <cstdint>

#include "math/matrix.hpp"
#include "math/Vector.hpp"

#include <utilities/pad.hpp>
#include <utilities/vfunc.hpp>

#define FLOW_OUTGOING	0
#define FLOW_INCOMING	1
#define MAX_FLOWS		2

class INetChannel
{
public:
	VFUNC(char*, getAddress, 1, (), (this));
	VFUNC(float, getTime, 2, (), (this));
	VFUNC(bool, isLoopBack, 6, (), (this));
	VFUNC(bool, isPlayBack, 8, (), (this));
	VFUNC(float, getLatency, 9, (int flow), (this, flow));
	PAD(24);
	int m_outSequenceNr;
	int m_inSequenceNr;
	int m_outSequenceNrAck;
	int m_outReliableState;
	int m_inReliableState;
	int m_chockedPackets;
};

struct PlayerInfo_t
{
	PAD(8);
	union
	{
		uint64_t m_steamID64;
		struct
		{
			int m_xuidlow;
			int m_xuidhigh;
		};
	};
	char m_name[128];
	int m_userID;
	char m_guid[33];
	uint32_t m_friendsID;
	char m_friendsName[128];
	bool m_fakePlayer;
	bool m_ishltv;
	size_t m_cutomFiles[4];
	uint8_t m_filesDownloaded;
};

class IVEngineClient
{
public:
	VFUNC(bool, isInGame, 26, (), (this));
	VFUNC(bool, isConnected, 27, (), (this));
	VFUNC(int, getLocalPlayer, 12, (), (this));
	VFUNC(int, getPlayerID, 9, (int id), (this, id));
	VFUNC(void, setViewAngles, 19, (const Vec3& angles), (this, std::cref(angles)));
	VFUNC(void, getViewAngles, 18, (Vec3& angles), (this, std::ref(angles)));
	VFUNC(int, getMaxClients, 20, (), (this));
	VFUNC(int, getPlayerInfo, 8, (int index, PlayerInfo_t* info), (this, index, info));
	VFUNC(void, getScreenSize, 5, (int& width, int& height), (this, std::ref(width), std::ref(height)));
	VFUNC(void, executeClientCMD, 114, (const char* cmd, char delay = 0), (this, cmd, delay));
	VFUNC(Matrix4x4&, worldToScreenMatrix, 37, (), (this));
	VFUNC(const char*, getLevelName, 53, (), (this));
	VFUNC(INetChannel*, getNameNetChannel, 78, (), (this));
	VFUNC(bool, isTakingScreenshot, 92, (), (this));
	VFUNC(bool, isConsoleVisible, 11, (), (this));
};
