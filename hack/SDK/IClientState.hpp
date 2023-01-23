#pragma once

#include "helpers/pad.hpp"

class INetChannel;
class IClientState
{
	PAD(156);
	INetChannel* m_netChannel;
	PAD(212);
	int m_deltaTick;
	PAD(19380);
	int m_lastOutGoingCommand;
	int m_chockedCommands;
	int m_lastCommandAck;
	int m_commandAck;
};