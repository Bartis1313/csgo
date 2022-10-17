#pragma once

#include <classes/createMove.hpp>

#include <deque>

class CUserCmd;
class INetChannel;
class IConVar;

class FakeLatency : public CreateMovePrePredictionType
{
public:
	FakeLatency() :
		CreateMovePrePredictionType{}
	{}

public:
	static int runDatagram(INetChannel* netChannel, void* datagram);
protected:
	virtual void run([[maybe_unused]] CUserCmd* cmd) override;
	virtual void init() override;
private:
	static void addLatency(INetChannel* netChannel, float latency);
	static int CALL(INetChannel* netChannel, void* datagram);
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
	inline static std::deque<SequenceRecord> m_sequences;
	inline static IConVar* maxUnlag = nullptr;
};

GLOBAL_FEATURE(FakeLatency);