#pragma once

#include <utilities/pad.hpp>

class INetChannel;
class IClientState
{
	PAD(156);
	INetChannel* m_netChannel;
	PAD(216);
	int m_deltaTick;
	PAD(19380);
	int m_lastOutGoingCommand;
	int m_chockedCommands;
	int m_lastCommandAck;
};