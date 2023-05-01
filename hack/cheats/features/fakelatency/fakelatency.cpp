#include "fakelatency.hpp"

#include <SDK/IVEngineClient.hpp>
#include <SDK/ConVar.hpp>
#include <SDK/ICvar.hpp>
#include <SDK/interfaces/interfaces.hpp>
#include <config/vars.hpp>
#include <cheats/game/game.hpp>

#include <cheats/hooks/sendDatagram.hpp>
#include <cheats/hooks/createMove.hpp>

namespace
{
	struct FakeLatencyCM : hooks::CreateMove
	{
		FakeLatencyCM()
		{
			this->registerInit(fakeLatency::init);
			this->registerRunPrePrediction(fakeLatency::run);
		}
	} fakeLatCM;
}

namespace fakeLatency
{
	struct SequenceRecord
	{
		int inReliableState;
		int outReliableState;
		int m_sequenceNr;
		float m_curtime;
	};

	int m_lastSequence{ 0 };
	std::deque<SequenceRecord> m_sequences;
	IConVar* maxUnlag;

	void addLatency(INetChannel* netChannel, float latency);
	void updateSequences();
	void clearSequences();
}

void fakeLatency::init()
{
	maxUnlag = memory::interfaces::cvar->findVar("sv_maxunlag");
}

int fakeLatency::runDatagram(INetChannel* netChannel, void* datagram)
{
	int reliableStateBackup = netChannel->m_inReliableState;
	int sequenceNrBackup = netChannel->m_inSequenceNr;

	float maxLatency = std::max(0.0f, std::clamp(vars::misc->fakeLatency->amount / 1000.f, 0.f, maxUnlag->getFloat())
		- netChannel->getLatency(FLOW_OUTGOING));
	addLatency(netChannel, maxLatency);

	const auto ret = hooks::SendDatagram::getOriginal()(netChannel, datagram);

	netChannel->m_inReliableState = reliableStateBackup;
	netChannel->m_inSequenceNr = sequenceNrBackup;

	return ret;
}

void fakeLatency::run(CUserCmd* cmd)
{
	updateSequences();
}

void fakeLatency::updateSequences()
{
	if (!game::isAvailable())
		return;

	if (!vars::misc->fakeLatency->enabled)
	{
		clearSequences();
		return;
	}

	auto network = memory::interfaces::engine->getNameNetChannel();
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
				.inReliableState = network->m_inReliableState,
				.outReliableState = network->m_outReliableState,
				.m_sequenceNr = network->m_inSequenceNr,
				.m_curtime = game::serverTime()
			}
		);
	}

	if (m_sequences.size() > 2048)
		m_sequences.pop_back();
}

void fakeLatency::clearSequences()
{
	if (!m_sequences.empty())
	{
		m_lastSequence = 0;
		m_sequences.clear();
	}
}

void fakeLatency::addLatency(INetChannel* netChannel, float latency)
{
	for (auto& el : m_sequences)
	{
		if (game::serverTime() - el.m_curtime >= latency)
		{
			netChannel->m_inReliableState = el.inReliableState;
			netChannel->m_inSequenceNr = el.m_sequenceNr;
			break;
		}
	}
}
