#pragma once

#include "math/Vector.hpp"

enum SendPropType
{
    DPT_Int = 0,
    DPT_Float,
    DPT_Vector,
    DPT_VectorXY,
    DPT_String,
    DPT_Array,
    DPT_DataTable,
    DPT_Int64,
    DPT_NUMSendPropTypes
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
        Vector m_vector;
        __int64 m_int64;
    };
    SendPropType  m_type;
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

using RecvVarProxyFn = void(*)(CRecvProxyData&, void*, void*); // cdecl

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
