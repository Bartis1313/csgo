#pragma once
#include <cstdint>

#include "math/VMatrix.hpp"
#include "math/Vector.hpp"
#include "../utilities/utilities.hpp"

#define FLOW_OUTGOING	0		
#define FLOW_INCOMING	1
#define MAX_FLOWS		2

class INetChannelInfo
{
public:
	VFUNC(float, getLatency, 9, (int flow), (this, flow));
	PAD(44);
	int chockedPackets;
};

typedef struct playerInfo_t
{
	long long unknown;
	union
	{
		long long steamID64;
		struct
		{
			int xuidlow;
			int xuidhigh;
		};
	};
	char name[128];
	int userId;
	char guid[33];
	unsigned int friendsid;
	char friendsname[128];
	bool fakeplayer;
	bool ishltv;
	unsigned int customfiles[4];
	unsigned char filesdownloaded;
} playerInfo_t;

class IVEngineClient
{
public:
	VFUNC(bool, isInGame, 26, (), (this));
	VFUNC(bool, isConnected, 27, (), (this));
	VFUNC(int, getLocalPlayer, 12, (), (this));
	VFUNC(int, getPlayerID, 9, (int id), (this, id));
	// need to be const
	VFUNC(void, setViewAngles, 19, (const Vector& angles), (this, std::cref(angles)));
	VFUNC(void, getViewAngles, 18, (Vector& angles), (this, std::ref(angles)));
	VFUNC(int, getMaxClients, 20, (), (this));
	VFUNC(int, getPlayerInfo, 8, (int index, playerInfo_t* info), (this, index, info));
	VFUNC(void, getScreenSize, 5, (int& width, int& height), (this, std::ref(width), std::ref(height)));
	// this is not really this command, but another one, the delay is smth with bytes, go look ida parameters
	VFUNC(void, executeClientCMD, 114, (const char* cmd, char delay = 0), (this, cmd, 0));
	VFUNC(VMatrix&, worldToScreenMatrix, 37, (), (this));
	VFUNC(const char*, getLevelName, 53, (), (this));
	VFUNC(INetChannelInfo*, getNameNetChannel, 78, (), (this));
};
