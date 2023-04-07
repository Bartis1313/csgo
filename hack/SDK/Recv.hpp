#pragma once

#include "math/Vector.hpp"

enum SendPropType
{
	DPT_INT = 0,
	DPT_FLOAT,
	DPT_VECTOR,
	DPT_VECTOR2D,
	DPT_STRING,
	DPT_ARRAY,
	DPT_DATATABLE,
	DPT_INT64,
	DPT_SENDPROPTYPEMAX
};

class DVariant
{
public:
	union
	{
		float m_float;
		long m_int;
		char* m_string;
		void* m_data;
		Vec3 m_vector;
		int64_t m_int64;
	};
	SendPropType m_type;
};

class RecvTable;
class RecvProp;

class CRecvProxyData
{
public:
	const RecvProp* m_recvProp;
	DVariant m_value;
	int	m_element;
	int	m_objectID;
};

using RecvVarProxyFn = std::add_pointer_t<void __cdecl(CRecvProxyData&, void*, void*)>;

class RecvProp
{
public:
	char* m_varName;
	SendPropType m_recvType;
	int m_flags;
	int m_stringBufferSize;
	int m_insideArray;
	const void* m_extraData;
	RecvProp* m_arrayProp;
	void* m_arrayLengthProxy;
	RecvVarProxyFn m_proxyFn;
	void* m_dataTableProxyFn;
	RecvTable* m_dataTable;
	int m_offset;
	int m_elementStride;
	int m_elements;
	const char* m_parentArrayPropName;
};

class RecvTable
{
public:
	RecvProp* m_props;
	int m_propsNum;
	void* m_decoder;
	char* m_netTableName;
	bool m_initialized;
	bool m_inMainList;
};
