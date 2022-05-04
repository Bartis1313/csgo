#pragma once

#include "../../../SDK/math/matrix.hpp"
#include "../../../SDK/math/Vector.hpp"
#include "../../../SDK/vars.hpp"

#include <deque>
#include <array>

class CUserCmd;
struct Vector;
class INetChannel;

class Backtrack final
{
public:
	void run(CUserCmd* cmd);
	void update(int frame);
	void init();
	_NODISCARD bool isValid(float simtime) const;
	void addLatency(INetChannel* netChannel, float latency);
	void updateSequences();
private:
	_NODISCARD float getLerp() const;
	_NODISCARD float extraTicks() const;
private:
	void clearSequences();
	
	struct StoredRecord
	{
		float m_simtime = 0.0f;
		Vector m_head = {};
		// use origin to set abs or for whatever need
		Vector m_origin = {};
		std::array<Matrix3x4, BONE_USED_BY_HITBOX> m_matrix;
	};
	struct SequenceRecord
	{
		int m_inReliableState;
		int m_outReliableState;
		int m_sequenceNr;
		float _m_curtime;
	};
	int m_lastSequence = 0;
	std::array<std::deque<StoredRecord>, 65> m_records;
	std::deque<SequenceRecord> m_sequences;
	struct CorrectRecord
	{
		std::array<Matrix3x4 ,BONE_USED_BY_HITBOX> m_matrix;
		float m_correctSimtime = {};
		Vector m_origin = {};
		bool m_setup = false;

		void reset() { m_correctSimtime = {}; m_origin = {}; }
	};
	std::array<CorrectRecord, 65> m_correct;
public:
	_NODISCARD auto& getAllRecords() { return m_records; }
};

inline Backtrack backtrack;