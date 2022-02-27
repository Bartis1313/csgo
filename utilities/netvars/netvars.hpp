#pragma once
#include <cstdint>
#include <fstream>
#include <unordered_map>
#include "../../utilities/utilities.hpp"
#include "../../SDK/Recv.hpp"

// generate netvar address
// type - template type for return type
// name - name your function
// table - table name to search the netvar prop
// prop - prop name
#define NETVAR(type, name, table, prop) \
_NODISCARD std::add_lvalue_reference_t<type> name() { \
	static uintptr_t offset = netvarMan.getNetvar(XOR(table), XOR(prop)); \
	return *reinterpret_cast<std::add_pointer_t<type>>((uintptr_t)this + offset ); \
    }

// generate netvar, but address as the pointer
// type - template type for return type
// name - name your function
// table - table name to search the netvar prop
// prop - prop name
#define PTRNETVAR(type, name, table, prop) \
_NODISCARD std::add_pointer_t<type> name() { \
	static uintptr_t offset = netvarMan.getNetvar(XOR(table), XOR(prop)); \
	return reinterpret_cast<std::add_pointer_t<type>>((uintptr_t)this + offset); \
    }

// define literal offset
// type - template type for return type
// name - name your function
// offset - offset to add
#define OFFSET(type, name, offset) \
_NODISCARD std::add_lvalue_reference_t<type> name() { \
	return *reinterpret_cast<std::add_pointer_t<type>>((uintptr_t)this + offset); \
	}

// use like normal netvar, with bytes to add as last argument
// type - template type for return type
// name - name your function
// table - table name to search the netvar prop
// prop - prop name
// addr - extra offset to add
#define NETVAR_ADDR(type, name, table, prop, addr) \
_NODISCARD std::add_lvalue_reference_t<type> name() { \
	static uintptr_t offset = netvarMan.getNetvar(XOR(table), XOR(prop)); \
	return *reinterpret_cast<std::add_pointer_t<type>>((uintptr_t)this + offset + addr); \
	}

class NetvarManager
{
public:
	void init();
	uintptr_t getNetvar(const char* tableName, const char* propName) const;
	void dump();
private:
	std::unordered_map<std::string, RecvTable*> m_Tables;
	uintptr_t getProp(const char* tableName, const char* propName, RecvProp** prop = 0) const;
	uintptr_t getProp(RecvTable* recvTable, const char* propName, RecvProp** prop = 0) const;
	RecvTable* getTable(const char* tableName) const;
	std::string getType(RecvProp* recvTable) const;
	void dump(RecvTable* recvTable);
	std::ofstream file;
} inline netvarMan;