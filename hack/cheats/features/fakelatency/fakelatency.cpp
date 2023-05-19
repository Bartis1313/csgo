#include "fakelatency.hpp"

#include <SDK/IVEngineClient.hpp>
#include <SDK/ConVar.hpp>
#include <SDK/ICvar.hpp>
#include <SDK/interfaces/interfaces.hpp>
#include <config/vars.hpp>
#include <cheats/game/game.hpp>

#include <cheats/hooks/sendDatagram.hpp>
#include <cheats/hooks/createMove.hpp>

#include <deque>

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

	int lastSequence{ 0 };
	std::deque<SequenceRecord> sequences;
	IConVar* sv_maxunlag;

	void addLatency(INetChannel* netChannel, float latency);
	void updateSequences();
	void clearSequences();
}

void fakeLatency::init()
{
	sv_maxunlag = memory::interfaces::cvar->findVar("sv_maxunlag");
}

int fakeLatency::runDatagram(INetChannel* netChannel, void* datagram)
{
	int reliableStateBackup = netChannel->m_inReliableState;
	int sequenceNrBackup = netChannel->m_inSequenceNr;

	float maxLatency = std::max(0.0f, std::clamp(vars::misc->fakeLatency->amount / 1000.f, 0.f, sv_maxunlag->getFloat())
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

	if (lastSequence == 0)
		lastSequence = network->m_inSequenceNr;

	if (network->m_inSequenceNr > lastSequence)
	{
		lastSequence = network->m_inSequenceNr;
		sequences.emplace_front(
			SequenceRecord
			{
				.inReliableState = network->m_inReliableState,
				.outReliableState = network->m_outReliableState,
				.m_sequenceNr = network->m_inSequenceNr,
				.m_curtime = game::serverTime()
			}
		);
	}

	if (sequences.size() > 2048)
		sequences.pop_back();
}

void fakeLatency::clearSequences()
{
	if (!sequences.empty())
	{
		lastSequence = 0;
		sequences.clear();
	}
}

void fakeLatency::addLatency(INetChannel* netChannel, float latency)
{
	for (auto& el : sequences)
	{
		if (game::serverTime() - el.m_curtime >= latency)
		{
			netChannel->m_inReliableState = el.inReliableState;
			netChannel->m_inSequenceNr = el.m_sequenceNr;
			break;
		}
	}
}
