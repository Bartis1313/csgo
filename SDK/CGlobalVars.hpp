#pragma once

class CGlobalVarsBase
{
public:
	float m_realtime;
	int m_frameCount;
	float m_absoluteframetime;
	float m_absoluteframestarttimestddev;
	float m_curtime;
	float m_frametime;
	int m_maxClients;
	int m_tickCount;
	float m_intervalPerTick;
	float m_interpolationAmount;
	int m_simTicksThisFrame;
	int m_networkProtocol;
	void* m_saveData;
	bool m_client;
	bool m_remoteClient;
	int m_networkingTimestamp;
	int m_networkingTimestampRandomize;
};