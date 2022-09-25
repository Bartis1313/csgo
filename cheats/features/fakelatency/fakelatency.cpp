#include "fakelatency.hpp"

#include <SDK/IVEngineClient.hpp>
#include <SDK/interfaces/interfaces.hpp>
#include <config/vars.hpp>
#include <game/game.hpp>

void FakeLatency::init()
{

}

void FakeLatency::run(CUserCmd* cmd)
{
	updateSequences();
}

void FakeLatency::updateSequences()
{
	if (!game::isAvailable())
		return;

	if (!config.get<bool>(vars.bFakeLatency))
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
