#pragma once

#include <classes/createMove.hpp>

#include <deque>

class CUserCmd;
class INetChannel;

class FakeLatency : public CreateMovePrePredictionType
{
public:
	FakeLatency() :
		CreateMovePrePredictionType{}
	{}

	virtual void init();
	virtual void run([[maybe_unused]] CUserCmd* cmd);
	void addLatency(INetChannel* netChannel, float latency);
private:
	void updateSequences();
	void clearSequences();

	struct SequenceRecord
	{
		int m_inReliableState;
		int m_outReliableState;
		int m_sequenceNr;
		float m_curtime;
	};

	int m_lastSequence = 0;
	std::deque<SequenceRecord> m_sequences;
};

[[maybe_unused]] inline auto g_FakeLatency = FakeLatency{};