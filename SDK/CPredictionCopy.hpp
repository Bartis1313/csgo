#pragma once

#include "datamap.hpp"
#include <stack>

#define PC_DATA_PACKED true
#define PC_DATA_NORMAL false

typedef void (*FN_FIELD_COMPARE)(const char* classname, const char* fieldname, const char* fieldtype,
	bool networked, bool noterrorchecked, bool differs, bool withintolerance, const char* value);

enum
{
	SLOT_ORIGINALDATA = -1,
};

class CPredictionCopy
{
public:
	typedef enum
	{
		DIFFERS = 0,
		IDENTICAL,
		WITHINTOLERANCE,
	} difftype_t;

	typedef enum
	{
		TRANSFERDATA_COPYONLY = 0,  // Data copying only (uses runs)
		TRANSFERDATA_ERRORCHECK_NOSPEW, // Checks for errors, returns after first error found
		TRANSFERDATA_ERRORCHECK_SPEW,   // checks for errors, reports all errors to console
		TRANSFERDATA_ERRORCHECK_DESCRIBE, // used by hud_pdump, dumps values, etc, for all fields
	} optype_t;

	CPredictionCopy(int type, byte* dest, bool dest_packed, const byte* src, bool src_packed,
		optype_t opType, FN_FIELD_COMPARE func = NULL);

	int transferData(const char* operation, int entindex, DataMap_t* dmap);
private:
	optype_t m_OpType;
	int m_type;
	byte* m_dest;
	const byte* m_src;
	int m_destOffsetIndex;
	int	m_srcOffsetIndex;
	int	m_errorCount;
	int	m_entIndex;
	FN_FIELD_COMPARE m_fieldCompareFunc;
	const Typedescription_t* m_watchField;
	char const* m_operation;
	std::stack<const Typedescription_t*> m_fieldStack;
};
