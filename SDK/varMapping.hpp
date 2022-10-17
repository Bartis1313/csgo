#pragma once

#include "CUtlVector.hpp"

class VarMapEntry_t
{
public:
	uint16_t m_type;
	uint16_t m_needsToInterpolate;	// Set to false when this var doesn't
	// need Interpolate() called on it anymore.
	void* m_data;
	/*IInterpolatedVar**/ void* m_watcher;
};

struct VarMapping_t
{
	constexpr VarMapping_t() :
		m_interpolatedEntries{ 0 }
	{}

	CUtlVector<VarMapEntry_t> m_Entries;
	int	m_interpolatedEntries;
	float m_lastInterpolationTime;
};