#include "CPredictionCopy.hpp"

#include <gamememory/memory.hpp>

CPredictionCopy::CPredictionCopy(int type, byte* dest, bool dest_packed, const byte* src, bool src_packed, optype_t opType, FN_FIELD_COMPARE func)
{
	this->m_OpType = opType;
	this->m_type = type;
	this->m_dest = dest;
	this->m_src = src;
	m_destOffsetIndex = dest_packed ? TD_OFFSET_PACKED : TD_OFFSET_NORMAL;
	m_srcOffsetIndex = src_packed ? TD_OFFSET_PACKED : TD_OFFSET_NORMAL;

	m_errorCount = 0;
	m_entIndex = -1;

	m_watchField = NULL;
	m_fieldCompareFunc = func;
}

int CPredictionCopy::transferData(const char* operation, int entindex, DataMap_t* dmap)
{
	return memory::transferData()(this, operation, entindex, dmap);
}