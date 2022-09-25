#include "hooks.hpp"

#include "../features/fakelatency/fakelatency.hpp"

#include <SDK/IVEngineClient.hpp>
#include <SDK/ICvar.hpp>
#include <SDK/ConVar.hpp>
#include <SDK/interfaces/interfaces.hpp>
#include <config/vars.hpp>
#include <game/game.hpp>

int __fastcall hooks::sendDatagram::hooked(INetChannel* netChannel, void* edx, void* datagram)
{
	if (datagram || !config.get<bool>(vars.bFakeLatency) || !game::isAvailable())
		return original(netChannel, datagram);

	int reliableStateBackup = netChannel->m_inReliableState;
	int sequenceNrBackup = netChannel->m_inSequenceNr;

	const static auto sv_maxunlag = interfaces::cvar->findVar(XOR("sv_maxunlag"));

	float maxLatency = std::max(0.0f, std::clamp(config.get<float>(vars.fFakeLatency) / 1000.f, 0.f, sv_maxunlag->getFloat()) - netChannel->getLatency(FLOW_OUTGOING));
	g_FakeLatency.addLatency(netChannel, maxLatency);

	const auto ret = original(netChannel, datagram);

	netChannel->m_inReliableState = reliableStateBackup;
	netChannel->m_inSequenceNr = sequenceNrBackup;

	return ret;
}