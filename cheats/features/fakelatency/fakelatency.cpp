#include "fakelatency.hpp"

#include <SDK/IVEngineClient.hpp>
#include <SDK/ConVar.hpp>
#include <SDK/ICvar.hpp>
#include <SDK/interfaces/interfaces.hpp>
#include <config/vars.hpp>
#include <game/game.hpp>
#include <hooks/hooks.hpp>

void FakeLatency::init()
{
	maxUnlag = interfaces::cvar->findVar(XOR("sv_maxunlag"));
}

int FakeLatency::runDatagram(INetChannel* netChannel, void* datagram)
{
	if (datagram || !vars::misc->fakeLatency->enabled || !game::isAvailable())
		return CALL(netChannel, datagram);

	int reliableStateBackup = netChannel->m_inReliableState;
	int sequenceNrBackup = netChannel->m_inSequenceNr;

	float maxLatency = std::max(0.0f, std::clamp(vars::misc->fakeLatency->amount / 1000.f, 0.f, maxUnlag->getFloat())
		- netChannel->getLatency(FLOW_OUTGOING));
	addLatency(netChannel, maxLatency);

	const auto ret = CALL(netChannel, datagram);

	netChannel->m_inReliableState = reliableStateBackup;
	netChannel->m_inSequenceNr = sequenceNrBackup;

	return ret;
}

void FakeLatency::run(CUserCmd* cmd)
{
	updateSequences();
}

int FakeLatency::CALL(INetChannel* netChannel, void* datagram)
{
	return hooks::sendDatagram::original(netChannel, datagram);
}

void FakeLatency::updateSequences()
{
	if (!game::isAvailable())
		return;

	if (!vars::misc->fakeLatency->enabled)
	{
		clearSequences();
		return;
	}

	auto network = interfaces::engine->getNameNetChannel();
	if (!network)
		return;

	if (m_lastSequence == 0)
		m_lastSequence = network->m_inSequenceNr;

	if (network->m_inSequenceNr > m_lastSequence)
	{
		m_lastSequence = network->m_inSequenceNr;
		m_sequences.emplace_front(
			SequenceRecord
			{
					network->m_inReliableState,
					network->m_outReliableState,
					network->m_inSequenceNr,
					game::serverTime()
			}
		);
	}

	if (m_sequences.size() > 2048)
		m_sequences.pop_back();
}

void FakeLatency::clearSequences()
{
	if (!m_sequences.empty())
	{
		m_lastSequence = 0;
		m_sequences.clear();
	}
}

void FakeLatency::addLatency(INetChannel* netChannel, float latency)
{
	for (auto& el : m_sequences)
	{
		if (game::serverTime() - el.m_curtime >= latency)
		{
			netChannel->m_inReliableState = el.m_inReliableState;
			netChannel->m_inSequenceNr = el.m_sequenceNr;
			break;
		}
	}
}
