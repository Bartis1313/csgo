#pragma once

#include "../../../SDK/math/matrix.hpp"
#include "../../../SDK/math/Vector.hpp"
#include "../../../SDK/vars.hpp"

#include <deque>
#include <array>

class CUserCmd;
struct Vector;

class Backtrack final
{
public:
	void run(CUserCmd* cmd);
	void update();
	void init();
	_NODISCARD bool isValid(float simtime) const;
private:
	_NODISCARD float getLerp() const;
	_NODISCARD float extraTicks() const;
private:
	struct StoredRecord
	{
		float m_simtime = 0.0f;
		Vector m_head = {};
		// use origin to set abs or for whatever need
		Vector m_origin = {};
		Matrix3x4 m_matrix[BONE_USED_BY_HITBOX];
	};
	std::array<std::deque<StoredRecord>, 65> m_records;
public:
	_NODISCARD auto& getAllRecords() { return m_records; }
};

inline Backtrack backtrack;